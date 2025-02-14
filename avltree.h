// Copyright (c) 2025 Dietfrid Mali
// This software is licensed under the MIT License.
// See the LICENSE file for more details.

#ifndef AVLTREE_H
#	define AVLTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

	typedef int (*avlDataComparator_t)(const void*, const void*);

	typedef bool (*nodeProcessor_t) (void* const);

	typedef struct avlTreeNode {
		struct avlTreeNode* left;
		struct avlTreeNode* right;
		int8_t				balance;
		void*				value;
	} avlTreeNode_t;

	typedef struct {
		avlTreeNode_t*		root;
		avlTreeNode_t*		current;
		void*				value;
		char*				typeName;
		int					typeSize;
		avlDataComparator_t comparator;
		bool				isDuplicate;
		bool				branchHasChanged;
		bool				deleteValues;
	} avlTreeDescriptor_t;

	avlTreeDescriptor_t* avltree_create(char* typeName, int typeSize, valueComparator_t comparator);

	bool avltree_insert(avlTreeDescriptor_t* avlTree, void* value);

	bool avltree_delete(avlTreeDescriptor_t* avlTree, void* key, bool deleteValues);

	bool avltree_replace(avlTreeDescriptor_t* avlTree, void* oldValue, void* newValue, bool deleteValue);

	void* avltree_find(avlTreeDescriptor_t* avlTree, void* key);

	bool avltree_walk(avlTreeDescriptor_t* avlTree, nodeProcessor_t processNode, bool reverse);

	bool avltree_is_empty(avlTreeDescriptor_t* avlTree);

#ifdef __cplusplus
}
#endif

#endif // AVLTREE_H defined
