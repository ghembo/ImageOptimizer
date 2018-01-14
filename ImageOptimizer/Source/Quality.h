#ifndef Quality_h__
#define Quality_h__

using Quality = unsigned int;

class  QualityRange
{
public:
	QualityRange(Quality minimumQuality, Quality maximumQuality) : m_minimumQuality{ minimumQuality }, m_maximumQuality{ maximumQuality } {};

	Quality GetMinimum() { return m_minimumQuality; };
	Quality GetMaximum() { return m_maximumQuality; };
	
private:
	Quality m_minimumQuality;
	Quality m_maximumQuality;
};

#endif // Quality_h__