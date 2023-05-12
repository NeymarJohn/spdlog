#pragma once

#include <array>
#include <vector>
#include <algorithm>

// Fast memory storage on the stack when possible or in std::vector
namespace c11log
{
namespace details
{



template<unsigned short STACK_SIZE = 128>
class stack_buf
{
public:
	using bufpair_t = std::pair<const char*, std::size_t>;
	using iterator = char const*;
	static constexpr unsigned short stack_size = STACK_SIZE;
    stack_buf() :_v(), _stack_size(0) {}
    ~stack_buf() = default;

    stack_buf& operator=(const stack_buf& other) = delete;

    stack_buf(const stack_buf& other):stack_buf(other, delegate_copy_move{})
    {}

    stack_buf(stack_buf&& other):stack_buf(other, delegate_copy_move{})
    {        
        other.clear();
    }

    void append(const char* buf, std::size_t buf_size)
    {
        //If we are aleady using _v, forget about the stack
        if (vector_used())
        {
            _v.insert(_v.end(), buf, buf + buf_size);
        }
        //Try use the stack
        else
        {
            if (_stack_size + buf_size <= STACK_SIZE)
            {
                std::memcpy(&_stack_array[_stack_size], buf, buf_size);
                _stack_size += buf_size;
            }
            //Not enough stack space. Copy all to _v
            else
            {
                _v.reserve(_stack_size + buf_size);                
				_v.insert(_v.end(), _stack_array.begin(), _stack_array.begin() + _stack_size);
                _v.insert(_v.end(), buf, buf + buf_size);
            }
        }
    }

	
    void clear()
    {
        _stack_size = 0;
        _v.clear();
    }

    bufpair_t get() const
    {
        if (vector_used())
            return bufpair_t(_v.data(), _v.size());
        else
            return bufpair_t(_stack_array.data(), _stack_size);
    }

	iterator begin() const
	{
		return get().first;
	}

	iterator end() const
	{
		bufpair_t bpair = get();
		return bpair.first + bpair.second;
	}

    std::size_t size() const
    {
       return get().second;
    }

private:
	struct delegate_copy_move {};
	template<class T1>
	stack_buf(T1&& other, delegate_copy_move)
	{
		_stack_size = other._stack_size;
        if (other.vector_used())
            _v = std::forward<T1>(other)._v;
        else
            std::copy_n(other._stack_array.begin(), other._stack_size, _stack_array.begin());        
    }

	bool vector_used() const
	{
		return !(_v.empty());
	}
	
    std::vector<char> _v;
    std::array<char, STACK_SIZE> _stack_array;
    std::size_t _stack_size;
};

}
} //namespace c11log { namespace details {
