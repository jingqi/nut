/**
 * @file -
 * @author jingqi
 * @date 2012-04-03
 * @last-edit 2012-04-03 18:14:47 jingqi
 */

#ifndef ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_
#define ___HEADFILE_0D8E9B0B_ACDC_4FD5_A0BE_71D75F7A5EFE_

#include <assert.h>
#include <stdint.h>

#include "byte_array_number.hpp"

namespace nut
{

/**
 * 无限大整数
 */
class BigInteger
{
	bool m_positive;
	uint8_t *m_buffer; // 缓冲区
	size_t m_buffer_len; // 缓冲区长度
	size_t m_significant_len; // 缓冲区有效位长度

private:
	/** 释放内存 */
	void free_mem()
	{
		if (NULL != m_buffer)
			::free(m_buffer);
		m_buffer = NULL;
		m_buffer_len = 0;
		m_significant_len = 0;
	}

	/** 重新分配内存 */
	void ensure_cap(size_t size_needed)
	{
		if (m_buffer_len >= size_needed)
			return;

		int newcap = m_buffer_len * 3 / 2;
		if (newcap < size_needed)
			newcap = size_needed;
		uint8_t *newbuf = (uint8_t*) ::malloc(sizeof(uint8_t) * newcap);
		if (NULL != m_buffer && m_significant_len > 0)
			::memcpy(newbuf, m_buffer, m_significant_len);
		if (NULL != m_buffer)
			::free(m_buffer);
		m_buffer = newbuf;
		m_buffer_len = newcap;
	}

	inline void adjust_significant_len()
	{
		m_significant_len = significant_size_unsigned(m_buffer, m_significant_len);
	}

public:
	BigInteger()
		: m_positive(true), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
	{}

	explicit BigInteger(long v)
		: m_positive(v >= 0), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
	{
		ensure_cap(sizeof(v));
		if (!m_positive)
			v = -v; // 即使取反的过程中溢出也没问题
		::memcpy(m_buffer, &v, sizeof(v));
		m_significant_len = sizeof(v);
	}

	BigInteger(const uint8_t *buf, size_t len, bool withSign)
		: m_positive(true), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
	{
		assert(NULL != buf && len > 0);
		ensure_cap(len);
		::memcpy(m_buffer, buf, len);
		if (withSign && !is_positive_signed(buf, len))
		{
			opposite_signed(m_buffer, m_buffer, len);
			m_positive = false;
		}
		m_significant_len = len;
		adjust_significant_len();
	}

	BigInteger(const BigInteger& x)
		: m_positive(x.m_positive), m_buffer(NULL), m_buffer_len(0), m_significant_len(0)
	{
		ensure_cap(x.m_significant_len);
		if (x.m_significant_len > 0)
			::memcpy(m_buffer, x.m_buffer, x.m_significant_len);
		m_significant_len = x.m_significant_len;
	}

	~BigInteger()
	{
		free_mem();
	}

public:
	BigInteger& operator=(const BigInteger& x)
	{
		ensure_cap(x.m_significant_len);
		if (x.m_significant_len > 0)
			::memcpy(m_buffer, x.m_buffer, x.m_significant_len);
		m_positive = x.m_positive;
		m_significant_len = x.m_significant_len;
	}

	bool operator==(const BigInteger& x) const
	{
		const bool zero1 = is_zero(), zero2 = x.is_zero();
		if (zero1 && zero2)
			return true;
		else if (zero1 || zero2)
			return false;
		else if (m_positive != x.m_positive)
			return false;

		const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
		for (register size_t i = 0; i < max_sig; ++i)
		{
			const uint8_t op1 = (i < m_significant_len ? m_buffer[i] : 0);
			const uint8_t op2 = (i < x.m_significant_len ? x.m_buffer[i] : 0);
			if (op1 != op2)
				return false;
		}
		return true;
	}

	bool operator!=(const BigInteger& x) const
	{
		return !(*this == x);
	}

	bool operator<(const BigInteger& x) const
	{
		const bool zero1 = is_zero(), zero2 = x.is_zero();
		if (zero1 && zero2)
			return false;
		else if (zero1)
			return x.m_positive;
		else if (zero2)
			return !m_positive;
		else if (m_positive != x.m_positive)
			return x.m_positive;

		// 同号非零值进行比较
		const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
		for (register int i = max_sig - 1; i >= 0; ++i)
		{
			const uint8_t op1 = (i < m_significant_len ? m_buffer[i] : 0);
			const uint8_t op2 = (i < x.m_significant_len ? x.m_buffer[i] : 0);
			if (op1 != op2)
				return (m_positive && op1 < op2) || (!m_positive && op1 > op2);
		}
		return false;
	}

	bool operator>(const BigInteger& x) const
	{
		return x < *this;
	}

	bool operator<=(const BigInteger& x) const
	{
		return !(x < *this);
	}

	bool operator>=(const BigInteger& x) const
	{
		return !(*this < x);
	}

	BigInteger operator+(const BigInteger& x) const
	{
		const size_t max_sig = (m_significant_len > x.m_significant_len ? m_significant_len : x.m_significant_len);
		BigInteger ret;
		ret.ensure_cap(max_sig + 1);
		if (x.m_positive == m_positive)
		{
			add_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_sig + 1);
			ret.m_significant_len = max_sig + 1;
			ret.m_positive = m_positive;
			return ret;
		}
		else
		{
			sub_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, max_sig + 1);
			ret.m_significant_len = max_sig + 1;
			ret.m_positive = m_positive;
			if (!is_positive_signed(ret.m_buffer, max_sig + 1))
			{
				opposite_assign_signed(ret.m_buffer, max_sig + 1);
				ret.m_positive = !m_positive;
			}
		}
		ret.adjust_significant_len();
		return ret;
	}

	BigInteger operator-(const BigInteger& x) const
	{
		return *this + (-x);
	}

	BigInteger operator-() const
	{
		BigInteger ret(*this);
		ret.m_positive = !m_positive;
		return ret;
	}

	BigInteger operator*(const BigInteger& x) const
	{
		BigInteger ret;
		ret.ensure_cap(m_significant_len + x.m_significant_len);
		multiply_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len + x.m_significant_len);
		ret.m_significant_len = m_significant_len + x.m_significant_len;
		ret.adjust_significant_len();
		ret.m_positive = ((m_positive && x.m_positive) || (!m_positive && !x.m_positive));
		return ret;
	}

	BigInteger operator/(const BigInteger& x) const
	{
		BigInteger ret;
		ret.ensure_cap(m_significant_len);
		divide_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, ret.m_buffer, m_significant_len, NULL, 0);
		ret.m_significant_len = m_significant_len;
		ret.m_positive = ((m_positive && x.m_positive) || (!m_positive && !x.m_positive));
		ret.adjust_significant_len();
		return ret;
	}

	BigInteger operator%(const BigInteger& x) const
	{
		BigInteger ret;
		ret.ensure_cap(m_significant_len);
		divide_unsigned(m_buffer, m_significant_len, x.m_buffer, x.m_significant_len, NULL, 0, ret.m_buffer, m_significant_len);
		ret.m_significant_len = m_significant_len;
		ret.m_positive = ((m_positive && x.m_positive) || (!m_positive && !x.m_positive));
		ret.adjust_significant_len();
		return ret;
	}

	BigInteger& operator+=(const BigInteger& x)
	{
		*this = *this + x;
		return *this;
	}

	BigInteger& operator-=(const BigInteger& x)
	{
		*this = *this - x;
		return *this;
	}

	BigInteger& operator*=(const BigInteger& x)
	{
		*this = *this * x;
		return *this;
	}

	BigInteger& operator/=(const BigInteger& x)
	{
		*this = *this / x;
		return *this;
	}

	BigInteger& operator%=(const BigInteger& x)
	{
		*this = *this % x;
		return *this;
	}

	BigInteger& operator++()
	{
		*this = *this + BigInteger(1);
		return *this;
	}

	BigInteger operator++(int)
	{
		BigInteger ret(*this);
		++*this;
		return ret;
	}

	BigInteger& operator--()
	{
		*this = *this - BigInteger(1);
		return *this;
	}

	BigInteger operator--(int)
	{
		BigInteger ret(*this);
		--*this;
		return ret;
	}

	BigInteger operator<<(size_t count) const
	{
		BigInteger ret(*this);
		ret.ensure_cap(m_significant_len + count / 8 + 1);
		shift_left_assign(ret.m_buffer, m_significant_len + count / 8 + 1, count);
		ret.m_significant_len = m_significant_len + count / 8 + 1;
		ret.adjust_significant_len();
		return ret;
	}

	BigInteger operator>>(size_t count) const
	{
		BigInteger ret(*this);
		shift_right_assign_unsigned(ret.m_buffer, m_significant_len, count);
		ret.m_significant_len = m_significant_len + count / 8 + 1;
		ret.adjust_significant_len();
		return ret;
	}
	
	BigInteger operator<<=(size_t count)
	{
		*this = *this << count;
		return *this;
	}

	BigInteger operator>>=(size_t count)
	{
		*this = *this >> count;
		return *this;
	}

public:
	void clear()
	{
		m_significant_len = 0;
	}

	bool is_zero() const
	{
		assert((m_buffer == NULL && m_buffer_len == 0 && m_significant_len == 0) ||
			(m_buffer != NULL && m_buffer_len > 0 && m_significant_len <= m_buffer_len));
		for (register size_t i = 0; i < m_significant_len; ++i)
			if (0 != m_buffer[i])
				return false;
		return true;
	}

	bool is_positive() const
	{
		return m_positive || is_zero();
	}

	long long_value() const
	{
		long ret = 0;
		::memcpy(&ret, m_buffer, sizeof(ret));
		return (m_positive ? ret : -ret);
	}
};

}

#endif /* head file guarder */

