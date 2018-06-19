/*
 * DoublyLinkedNode_tmpdecl.h
 *
 *  Created on: Apr 17, 2018
 *      Author: 13774
 */

#ifndef INCLUDE_DATA_DOUBLYLINKEDNODE_FORWARD_H_
#define INCLUDE_DATA_DOUBLYLINKEDNODE_FORWARD_H_



#include <new>
#include <type_traits>
#include <generic/programming.h>


template <class T>
class DoublyLinkedNode{
public:
	template <class...Args>
	DoublyLinkedNode(DoublyLinkedNode *next,DoublyLinkedNode *previous,Args && ... args);
	DELETE_COPY(DoublyLinkedNode);
	DoublyLinkedNode(DoublyLinkedNode &&rhs);
	DoublyLinkedNode & operator=(DoublyLinkedNode &&rhs);
	~DoublyLinkedNode();


	template <class...Args>
	void        data(Args && ... args);

	// 对于size <= sizeof(size_t)的简单类型，返回值即可
	// 但是用户可以选择对于值较小的类型仍然返回引用
	// 其他类型需要返回const引用
	T dataCopy()const { return _data;}
	T & dataRef()  { return _data;}
	const T & dataRef() const { return _data;}

	DoublyLinkedNode* next();
	const DoublyLinkedNode* next() const;
	void next(DoublyLinkedNode* node);

	DoublyLinkedNode* previous();
	const DoublyLinkedNode* previous() const;
	void previous(DoublyLinkedNode* node);

	// no check
	void insertAfter(DoublyLinkedNode *node);
	// no check
	void insertBefore(DoublyLinkedNode *node);

	DoublyLinkedNode * removeAfter();
	DoublyLinkedNode * removeBefore();
	DoublyLinkedNode * removeSelf();

private:
	T				  _data;
	DoublyLinkedNode *_next;
	DoublyLinkedNode *_previous;
};





#endif /* INCLUDE_DATA_DOUBLYLINKEDNODE_FORWARD_H_ */
