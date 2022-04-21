#ifndef CONSTANTELEMENTCOUNTGROWTH_
#define CONSTANTELEMENTCOUNTGROWTH_
#include <cstdint>

class ConstantElementCountGrowth
{
public:
	ConstantElementCountGrowth(uint32_t preAllocate = 16u, uint32_t grow = 16u)
		:
		m_PreAllocate(preAllocate),
		m_NumToGrow(grow)
	{}

protected:
	uint32_t GetNumToPreAllocate() const
	{
		return m_PreAllocate;
	}

	uint32_t GetNumToGrow(uint32_t numAlloced) const
	{
		return m_NumToGrow;
	}

private:
	uint32_t m_PreAllocate;
	uint32_t m_NumToGrow;
};
#endif // CONSTANTELEMENTCOUNTGROWTH_
