using System;
using System.IO.Ports;

namespace Acq
{
    [Serializable]
    public class Profile
    {
        public string Name { get; set; }

        public int BaudRate { get; set; }

        public int DataBits { get; set; }

        public StopBits StopBits { get; set; }

        public Parity Parity { get; set; }
    }
}
