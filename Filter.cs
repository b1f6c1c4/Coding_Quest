using System.Collections.Generic;
using System.Linq;

namespace Acq
{
    internal class Filter
    {
        private readonly object m_Lock = new object();

        private readonly List<double> m_List = new List<double>();

        public double Mean { get; private set; }

        public double Variance { get; private set; }

        public void Tick()
        {
            lock (m_Lock)
            {
                if (m_List.Count > 1)
                {
                    Mean = m_List.Average();
                    Variance = m_List.Sum(d => (d - Mean) * (d - Mean)) / (m_List.Count - 1);
                }
                else if (m_List.Count > 0)
                {
                    Mean = m_List.Average();
                    Variance = double.NaN;
                }
                else
                {
                    Mean = double.NaN;
                    Variance = double.NaN;
                }
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
