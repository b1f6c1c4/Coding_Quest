using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace Acq
{
    public partial class Form1 : Form
    {
        private readonly AsyncSerialPort m_Com;

        private StreamWriter m_StreamWriter;

        private int m_Count;
        private readonly Profile m_Profile;

        public Form1()
        {
            InitializeComponent();

            foreach (var portName in SerialPort.GetPortNames())
                listBox1.Items.Add(portName);

            button2.Enabled = false;

            m_Profile = new Profile
                            {
                                BaudRate = 38400,
                                DataBits = 8,
                                Name = listBox1.Text,
                                Parity = Parity.None,
                                StopBits = StopBits.One
                            };
            m_Com = new AsyncSerialPort(m_Profile, 5);
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
            if (package[0] != 0x00)
                return false;

            var t = package[1];
            t += package[2];
            t += package[3];
            t += package[4];
            if (t != package[5])
                return true;

            var data = (package[1] << 24) | (package[2] << 16) | (package[3] << 8) | (package[4]);
            m_StreamWriter.WriteLine(data);

            m_Count++;

            return true;
        }
    }
}
