#ifndef KE_UNROLLEDLIST_H
#define KE_UNROLLEDLIST_H

#include <new>

template<typename Type>
class UnrolledList
{
private:
	struct m_Node 
	{
		Type value;
		m_Node* prev;
		m_Node* next;
	};

	struct m_Arena 
	{
		m_Node* start;
		size_t size;
		m_Arena* next;
	};

	size_t m_size; 
	size_t m_capacity;

	m_Arena* m_headArena; 
	m_Arena* m_lastArena;

	m_Node* m_stackPtr; 
	m_Node m_headNode; 
	m_Node* m_lastNode; 
	m_Node* m_virtualLastNode;

	void f_setLastNode() 
	{
		if(!m_lastNode->next) 
		{
			if(m_size == m_capacity) 
			{
				m_lastArena->next = new m_Arena{(m_Node*)::operator new(m_capacity * sizeof(m_Node)), 2 * m_capacity, nullptr};
				m_lastArena = m_lastArena->next;
				m_stackPtr = m_lastArena->start;

				m_capacity *= 2;
			}

			m_lastNode = m_stackPtr++;
			m_virtualLastNode = m_lastNode;
		}
		else
		{
			m_lastNode = m_lastNode->next;
		}
	}

public:
	class Iterator 
	{
		friend class UnrolledList<Type>;

	private:
		m_Node* m_ptr;

	public:
		Iterator() :
			m_ptr(nullptr) 
		{}

		bool operator==(Iterator other) const 
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(Iterator other) const 
		{
			return m_ptr != other.m_ptr;
		}

		bool operator!=(std::nullptr_t nullPointer) const 
		{
			return m_ptr != nullPointer;
		}

		Iterator operator++() 
		{
			m_ptr = m_ptr->next;
			return *this;
		}

		Iterator operator++(int) 
		{
			Iterator ret = *this;
			m_ptr = m_ptr->next;
			return ret;
		}

		Iterator operator--() 
		{
			m_ptr = m_ptr->prev;
			return *this;
		}

		Iterator operator--(int) 
		{
			Iterator ret = *this;
			m_ptr = m_ptr->prev;
			return ret;
		}

		Type& operator*() const 
		{
			return m_ptr->value;
		}

		Type* operator->() const 
		{
			return &m_ptr->value;
		}

		Iterator next() const 
		{
			Iterator ret{};
			ret.m_ptr = m_ptr->next;
			return ret;
		}

		Iterator prev() const 
		{
			Iterator ret{};
			ret.m_ptr = m_ptr->prev;
			return ret;
		}
	};

	UnrolledList(size_t initCap) :
		m_size(0), m_capacity(initCap),
		m_headArena(new m_Arena{(m_Node*)::operator new(initCap * sizeof(m_Node)), initCap, nullptr}),
		m_lastArena(m_headArena),
		m_stackPtr(m_headArena->start), m_headNode{}, m_lastNode(&m_headNode), m_virtualLastNode(m_lastNode) 
	{}

	~UnrolledList() 
	{
		for(Type& obj : *this) 
		{
			obj.~Type();
		}

		for(m_Arena* arena = m_headArena; arena != nullptr; ) 
		{
			::operator delete(arena->start);
			m_Arena* next = arena->next;
			delete arena;
			arena = next;
		}
	}

	Iterator add(const Type& elem) 
	{
		return addInPlace(elem);
	}

	Iterator add(Type&& elem) 
	{
		return addInPlace(std::move(elem));
	}

	template<typename... ArgTypes>
	Iterator addInPlace(ArgTypes&&... args) 
	{
		m_Node* prevNode = m_lastNode;
		f_setLastNode();
		new (m_lastNode) m_Node{Type{std::forward<ArgTypes>(args)...}, prevNode, nullptr};

		prevNode->next = m_lastNode;

		m_size++;
		Iterator ret{}; ret.m_ptr = m_lastNode;
		return ret;
	}

	void remove(const Iterator removeIt) 
	{
		m_Node* toRemove = removeIt.m_ptr;

		if(toRemove->next) 
		{
			toRemove->prev->next = toRemove->next;
			toRemove->next->prev = toRemove->prev;
			toRemove->next = nullptr;
			toRemove->prev = nullptr;

			m_virtualLastNode->next = toRemove;
			m_virtualLastNode = toRemove;
		}
		else 
		{
			m_Node* newLast = m_lastNode->prev;
			newLast->next = nullptr;
			m_lastNode->prev = nullptr;
			m_lastNode = newLast;

			m_virtualLastNode = m_lastNode;
		}

		m_size--;
	}

	size_t size() const 
	{
		return m_size;
	}

	Iterator begin() const 
	{
		Iterator ret{};
		ret.m_ptr = m_headNode.next;
		return ret;
	}

	Iterator end() const 
	{
		Iterator ret{};
		ret.m_ptr = m_lastNode->next;
		return ret;
	}
};

#endif