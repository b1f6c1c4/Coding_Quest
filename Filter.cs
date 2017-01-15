using System.Collections.Generic;
using System.Linq;

namespace Acq
{
    internal class Filter
    {
        private readonly object m_Lock = new object();

        private readonly List<double> m_List = new List<double>();

        public double Val { get; private set; }

        public void Tick()
        {
            lock (m_Lock)
            {
                if (m_List.Count > 0)
                    Val = m_List.Average();
                else
                    Val = double.NaN;
                m_List.Clear();
            }
        }

        public void Data(double val)
        {
            lock (m_Lock)
            {
                m_List.Add(val);
            }
        }
    }
}
