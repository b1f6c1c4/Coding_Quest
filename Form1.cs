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

        private StreamWriter m_StreamWriter;

        private readonly Profile m_Profile;

        private int m_Count;

        private int m_Frequency;

        private Timer m_Timer;

        public Form1()
        {
            InitializeComponent();

            foreach (var portName in SerialPort.GetPortNames())
                listBox1.Items.Add(portName);
            listBox1.Text = (string)listBox1.Items[0];

            m_Timer = new Timer
                          {
                              Enabled = true,
                              Interval = 1000,
                          };
            m_Timer.Tick += (s, o) => m_Frequency = Interlocked.Exchange(ref m_Count, 0);

            button2.Enabled = false;

            m_Profile = new Profile
                            {
                                BaudRate = 38400,
                                DataBits = 8,
                                Name = listBox1.Text,
                                Parity = Parity.None,
                                StopBits = StopBits.One
                            };
            m_Com = new AsyncSerialPort(m_Profile, 13 * 5 + 2);
        }

        private delegate void SetLabel1Delegate(string text);
        private void SetLabel1(string text)
        {
            if (label1.InvokeRequired)
                label1.Invoke(new SetLabel1Delegate(SetLabel1), text);
            else
                label1.Text = text;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_Com.PackageArrived += ComOnPackageArrived;
            m_Profile.Name = listBox1.Text;
            m_Com.Open(m_Profile, new TimeSpan(0, 0, 30));

            m_StreamWriter = new StreamWriter("data.txt");

            button1.Enabled = false;
            button2.Enabled = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button1.Enabled = true;
            button2.Enabled = false;

            m_StreamWriter.Close();
            m_Com.Close();

            m_StreamWriter.Dispose();
            m_StreamWriter = null;
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
                var sys = TakeF(p);
                var state = TakeF(p);

                var uACre = Take(p);
                var uACim = Take(p);
                var uACrms = Take(p);
                var uACpha = Math.Atan2(uACim, uACre);

                var iACre = Take(p);
                var iACim = Take(p);
                var iACrms = Take(p);
                var iACpha = Math.Atan2(iACim, iACre);

                var lead = (iACpha - uACpha) % (Math.PI * 2);

                var uDC = Take(p);
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
                sb.AppendLine($"Sys Clk: {sys,10:x8}");
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
                sb.Append($"   {uACre,10:F6} + j{uACim,10:F6}V");
                sb.AppendLine($"   RMS {uACrms,10:F6}V Phase {uACpha / Math.PI * 180,10:F6}");
                sb.AppendLine("Iac:");
                sb.Append($"   {iACre,10:F6} + j{iACim,10:F6}A");
                sb.AppendLine($"   RMS {iACrms,10:F6}A Phase {iACpha / Math.PI * 180,10:F6}");
                sb.AppendLine(
                              lead > 0
                                  ? $"Iac-Uac: Lead {+lead / Math.PI * 180,10:F6}"
                                  : $"Iac-Uac: Lag  {-lead / Math.PI * 180,10:F6}");
                sb.AppendLine($"Udc: {uDC,10:F6}V");
                sb.AppendLine("Imp:");
                sb.Append($"   {impR,10:F6} + j{impX,10:F6}Ohm");
                sb.AppendLine($"   = {impZ,10:F6}Ohm");
                sb.AppendLine("Target Iac:");
                sb.Append($"   {itACre,10:F6} + j{itACim,10:F6}A");
                sb.AppendLine($"   RMS {itACrms,10:F6}A Phase {itACpha / Math.PI * 180,10:F6}");
                sb.Append(
                          tTanPhi > 0
                              ? $"Target cos phi: (Lead) {tCosPhi,10:F6}"
                              : $"Target cos phi: (Lag)  {tCosPhi,10:F6}");
                sb.AppendLine($"    tan phi={tTanPhi,10:F6}");
                sb.AppendLine($"Target Udc: {utDC,10:F6}");

                SetLabel1(sb.ToString());
                if (checkBox1.Checked)
                    ; //m_StreamWriter.WriteLine($"{iACre}");
            }
            catch (FormatException)
            {
                return false;
            }

            return true;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            m_Com.Send(Encoding.ASCII.GetBytes(textBox1.Text));
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
