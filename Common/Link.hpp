#pragma once

#ifndef _LINK_HPP_
#define _LINK_HPP_

template <typename type>
struct NODE 
{
	type  value; // ֵ
	NODE* prev;  // ��һ��ָ��
	NODE* next;  // ��һ��ָ��
};

template <typename type>
class Link
{
public:
	Link() {}
	// �������
	NODE<type>* Add(type value);
	// �Ƴ�����[flag=1->delete value flag=2->delete[] value]
	bool Remove(type v, int flag=0);
	// ����ͷ
	NODE<type>* Head();
	// ���õ�����ͷ
	NODE<type>* Reset();
	// ��ǰ����ָ��
	NODE<type>* Current();
	// ��һ������ָ��
	NODE<type>* Prev();
	// ��һ������ָ��
	NODE<type>* Next();
	// ���浱ǰ
	NODE<type>* Store();
	// �ָ�����
	NODE<type>* ReStore();
	// �Ƿ�������ͷ
	bool IsHead(NODE<type>* p=nullptr);
	// �ͷ��ڴ�[flag=1->delete value flag=2->delete[] value]
	void Release(int flag=0);
	// ��ǰ������
	int Count();
private:
	// ������
	int m_count = 0;

	// ����ͷ
	NODE<type>* m_head = nullptr;

	// ��ǰ����ָ��
	NODE<type>* m_current = nullptr;

	// ���һ������ָ��
	NODE<type>* m_last = nullptr;

	// ���������ָ��
	NODE<type>* m_store = nullptr;
protected:
public:

};

// �������
template<typename type>
NODE<type>* Link<type>::Add(type value)
{
#if 1
	NODE<type>* p = new NODE<type>;
	p->value = value;
	p->prev = m_last;
	p->next = nullptr;

	if (!m_head) {
		m_head = p;
		m_current = m_head;
		m_last = m_head;
	}
	else {
		m_last->next = p;
		m_last = p;
	}
	m_count++;

	return p;
#else
	return nullptr;
#endif
}

// �Ƴ�����
template<typename type>
inline bool Link<type>::Remove(type v, int flag)
{
	NODE<type>* eq = nullptr;
	for (NODE<type>* p = m_head; p; p = p->next) {
		if (p->value == v) {
			eq = p;
			break;
		}	
	}
	if (eq) {
		if (m_current == eq) // ��ǰָ�����Ҫɾ����ָ��
			m_current = eq->next;

		if (eq->prev) { // ���ǵ�һ��
			eq->prev->next = eq->next;
		}
		else { // ��һ��
			m_head = eq->next;
		}

		if (eq->next) { // ����һ��
			eq->next->prev = eq->prev;
		}
		if (eq == m_last) { // ���һ��
			m_last = eq->prev;
		}

		if (flag == 1)
			delete (void*)eq->value;
		else if (flag == 2)
			delete[] (void*)eq->value;
		delete eq;

		m_count--;
		return true;
	}
	return false;
}

// ����ͷ
template<typename type>
inline NODE<type>* Link<type>::Head()
{
	return m_head;
}

// ���õ�����ͷ
template<typename type>
NODE<type>* Link<type>::Reset()
{
	m_current = m_head;
	return m_current;
}

// ��ǰ����ָ��
template<typename type>
NODE<type>* Link<type>::Current()
{
	return m_current;
}

// ��һ������ָ��
template<typename type>
inline NODE<type>* Link<type>::Prev()
{
	if (!m_current)
		return nullptr;

	m_current = m_current->prev;
	return m_current;
}

// ��һ������ָ��
template<typename type>
NODE<type>* Link<type>::Next()
{
	if (!m_current)
		return nullptr;

	m_current = m_current->next;
	return m_current;
}

// ���浱ǰ
template<typename type>
inline NODE<type>* Link<type>::Store()
{
	m_store = m_current;
	return m_current;
}

// �ָ�����
template<typename type>
inline NODE<type>* Link<type>::ReStore()
{
	m_current = m_store;
	return m_current;
}

// �Ƿ�������ͷ
template<typename type>
inline bool Link<type>::IsHead(NODE<type>* p)
{
	if (p == nullptr)
		p = m_current;
	return p == m_head;
}

// �ͷ��ڴ�
template<typename type>
void Link<type>::Release(int flag)
{
	NODE<type>* p = m_head;
	while (p) {
		NODE<type>* next = p->next;
		if (flag == 1)
			delete (void*)p->value;
		else if (flag == 2)
			delete[] (void*)p->value;

		delete p;
		p = next;
	}
	m_count = 0;
}

// ��ǰ������
template<typename type>
inline int Link<type>::Count()
{
	return m_count;
}

#endif // !