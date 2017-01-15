using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Timer = System.Windows.Forms.Timer;

namespace Acq
{
    public partial class Form1 : Form
    {
        private readonly AsyncSerialPort m_Com;

        private readonly Profile m_Profile;

        private int m_Count;

        private int m_Frequency;

        private readonly Timer m_Timer;

        private readonly Filter m_UacFilter = new Filter();
        private readonly Filter m_IacFilter = new Filter();
        private readonly Filter m_UdcFilter = new Filter();

        private readonly Filter m_RawUacFilter = new Filter();
        private readonly Filter m_RawIacFilter = new Filter();
        private readonly Filter m_RawUdcFilter = new Filter();

        public Form1()
        {
            InitializeComponent();

            foreach (var portName in SerialPort.GetPortNames())
                listBox1.Items.Add(portName);
            listBox1.Text = (string)listBox1.Items[0];

            m_Timer = new Timer
                          {
                              Enabled = true,
                              Interval = 1000
                          };
            m_Timer.Tick += (s, o) =>
                            {
                                m_Frequency = Interlocked.Exchange(ref m_Count, 0);
                                m_UacFilter.Tick();
                                m_IacFilter.Tick();
                                m_UdcFilter.Tick();
                                m_RawUacFilter.Tick();
                                m_RawIacFilter.Tick();
                                m_RawUdcFilter.Tick();
                            };

            button2.Enabled = false;

            m_Profile = new Profile
                            {
                                BaudRate = 38400,
                                DataBits = 8,
                                Name = listBox1.Text,
                                Parity = Parity.None,
                                StopBits = StopBits.One
                            };
            m_Com = new AsyncSerialPort(m_Profile, 19 * 4 + 2);

            m_Com.PackageArrived += ComOnPackageArrived;
            m_Com.OpenPort += e => { SetButtonEnable(e != null); };
            m_Com.ClosePort += e => { SetButtonEnable(e == null); };
        }

        private delegate void SetLabel1Delegate(string text);

        private void SetLabel1(string text)
        {
            if (label1.InvokeRequired)
                label1.Invoke(new SetLabel1Delegate(SetLabel1), text);
            else
                label1.Text = text;
        }

        private delegate void SetButtonEnableDelegate(bool yes);

        private void SetButtonEnable(bool yes)
        {
            if (button1.InvokeRequired)
                button1.Invoke(new SetButtonEnableDelegate(SetButtonEnable), yes);
            else
            {
                button1.Enabled = yes;
                button2.Enabled = !yes;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_Profile.Name = listBox1.Text;
            m_Com.Open(m_Profile, new TimeSpan(0, 0, 30));

            button1.Enabled = false;
            button2.Enabled = false;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            button2.Enabled = false;

            m_Com.Close();
        }

        private bool ComOnPackageArrived(IList<byte> package)
        {
            if (package[0] != 0x5a)
                return false;
            if (package[1] != 0xa5)
                return false;

            var p = package.ToList();
            p.RemoveRange(0, 2);
            try
            {
                var rUac = Take(p);
                m_RawUacFilter.Data(rUac);
                var rIac = Take(p);
                m_RawIacFilter.Data(rIac);
                var rUdc = Take(p);
                m_RawUdcFilter.Data(rUdc);

                var sys = TakeF(p);
                var state = TakeF(p);

                var uACre = Take(p);
                var uACim = Take(p);
                var uACrms = Take(p);
                m_UacFilter.Data(uACrms);
                var uACpha = Math.Atan2(uACim, uACre);

                var iACre = Take(p);
                var iACim = Take(p);
                var iACrms = Take(p);
                m_IacFilter.Data(iACrms);
                var iACpha = Math.Atan2(iACim, iACre);

                var lead = (iACpha - uACpha) % (Math.PI * 2);

                var uDC = Take(p);
                m_UdcFilter.Data(uDC);
                var muDC = Take(p);
                var impR = Take(p);
                var impX = Take(p);
                var impZ = Math.Sqrt(impR * impR + impX * impX);

                var itACre = Take(p);
                var itACim = Take(p);
                var itACrms = Math.Sqrt(itACre * itACre + itACim * itACim);
                var itACpha = Math.Atan2(itACim, itACre);

                var tTanPhi = Take(p);
                var tCosPhi = 1 / Math.Sqrt(1 + tTanPhi * tTanPhi);
                var utDC = Take(p);

                var sb = new StringBuilder();
                Interlocked.Increment(ref m_Count);
                sb.AppendLine($"Update frequency: {m_Frequency,10}Hz");
                sb.AppendLine($"Sys Clk: {sys,15:x8}");
                switch (state)
                {
                    case 0:
                        sb.AppendLine("IDLE: All IGBT off");
                        break;
                    case 1:
                        sb.AppendLine("IMP:  Impedance Measurement mode");
                        break;
                    case 2:
                        sb.AppendLine("CURR: AC current closed loop");
                        break;
                    case 3:
                        sb.AppendLine("FULL: DC voltage closed loop + AC curr.");
                        break;
                }
                sb.AppendLine("Uac:");
                sb.Append($"   {uACre,15:F6} + j{uACim,15:F6}V");
                sb.AppendLine($"   RMS {uACrms,15:F6}V Phase {uACpha / Math.PI * 180,15:F6}");
                sb.AppendLine("Iac:");
                sb.Append($"   {iACre,15:F6} + j{iACim,15:F6}A");
                sb.AppendLine($"   RMS {iACrms,15:F6}A Phase {iACpha / Math.PI * 180,15:F6}");
                sb.AppendLine(
                              lead > 0
                                  ? $"Iac-Uac: Lead {+lead / Math.PI * 180,15:F6}"
                                  : $"Iac-Uac: Lag  {-lead / Math.PI * 180,15:F6}");
                sb.AppendLine($"Udc: {uDC,15:F6}V (IIR:) {muDC,15:F6}V");
                sb.AppendLine("Imp:");
                sb.Append($"   {impR,15:F6} + j{impX,15:F6}Ohm");
                sb.AppendLine($"   = {impZ,15:F6}Ohm");
                sb.AppendLine("Target Iac:");
                sb.Append($"   {itACre,15:F6} + j{itACim,15:F6}A");
                sb.AppendLine($"   RMS {itACrms,15:F6}A Phase {itACpha / Math.PI * 180,15:F6}");
                sb.Append(
                          tTanPhi > 0
                              ? $"Target cos phi: (Lead) {tCosPhi,15:F6}"
                              : $"Target cos phi: (Lag)  {tCosPhi,15:F6}");
                sb.AppendLine($"    tan phi={tTanPhi,15:F6}");
                sb.AppendLine($"Target Udc: {utDC,15:F6}");
                sb.AppendLine();
                sb.AppendLine();
                sb.AppendLine("Instant:");
                sb.AppendLine($"uac {rUac,15:F6}V avg.{m_RawUacFilter.Mean,20:F10}V");
                sb.AppendLine($" var.{m_RawUacFilter.Variance,20:F10} std.{Math.Sqrt(m_RawUacFilter.Variance),20:F10}");
                sb.AppendLine($"iac {rIac,15:F6}V avg.{m_RawIacFilter.Mean,20:F10}V");
                sb.AppendLine($" var.{m_RawIacFilter.Variance,20:F10} std.{Math.Sqrt(m_RawIacFilter.Variance),20:F10}");
                sb.AppendLine($"udc {rUdc,15:F6}V avg.{m_RawUdcFilter.Mean,20:F10}V");
                sb.AppendLine($" var.{m_RawUdcFilter.Variance,20:F10} std.{Math.Sqrt(m_RawUdcFilter.Variance),20:F10}");
                sb.AppendLine();
                sb.AppendLine();
                sb.AppendLine($"Avg. Uac {m_UacFilter.Mean,20:F10}V");
                sb.AppendLine($"Avg. Iac {m_UacFilter.Mean,20:F10}A");
                sb.AppendLine($"Avg. Udc {m_UacFilter.Mean,20:F10}A");

                SetLabel1(sb.ToString());
            }
            catch (FormatException)
            {
                return false;
            }

            return true;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            var s = textBox1.Text;
            if (string.IsNullOrEmpty(s))
                return;

            m_Com.Send(Encoding.ASCII.GetBytes(s));
            label2.Text = s;
            textBox1.Text = string.Empty;
        }

        private static int TakeF(List<byte> package)
        {
            var data = (package[0] << 24) | (package[1] << 16) | (package[2] << 8) | (package[3]);
            package.RemoveRange(0, 4);

            return data;
        }

        private static double Take(List<byte> package) => TakeF(package) / Math.Pow(2, 20);
    }
}
