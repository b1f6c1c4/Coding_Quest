using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace Acq
{
    public partial class Form1 : Form
    {
        private AsyncSerialPort m_Com;

        private StreamWriter m_StreamWriter;

        public Form1()
        {
            InitializeComponent();

            foreach (var portName in SerialPort.GetPortNames())
                listBox1.Items.Add(portName);

            button2.Enabled = false;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            m_Com = new AsyncSerialPort(
                new Profile
                    {
                        BaudRate = 38400,
                        DataBits = 8,
                        Name = listBox1.Text,
                        Parity = Parity.None,
                        StopBits = StopBits.One
                    },
                5);
            m_Com.PackageArrived += ComOnPackageArrived;

            m_StreamWriter = new StreamWriter("data.txt");

            button1.Enabled = false;
            button2.Enabled = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            button2.Enabled = false;

            m_Com.Close();
            m_StreamWriter.Close();

            Environment.Exit(0);
        }

        private bool ComOnPackageArrived(IList<byte> package)
        {
            if (package[0] != 0x5a)
                return false;

            var data = (package[1] << 24) | (package[2] << 16) | (package[3] << 8) | (package[4]);
            m_StreamWriter.WriteLine(data);

            return true;
        }
    }
}
