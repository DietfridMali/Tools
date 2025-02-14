avl_tree implements a type independent AVL tree manager.

The assumption made in this implementation is that each tree node's key is contained in the data stored in the node.
Hence, the key and the data types are the same. Whatever data type you use for the data to be stored in the tree,
you also use for passing keys to the avl tree manager.

The avl tree manager needs an external comparison function that can compare the data stored in the avl tree.
That function must be a function implemented on application side.

The function signature is 	

	typedef int (*avlDataComparator_t)(const void*, const void*)

i.e. the comparison function always receives pointers to the two data sets to be compared. Inside the function,
these pointers need to be cast to the proper pointer type of the data to be compared. The function has to return

	-1 if the first data set passed to it is less than the second one 
	+1 if the first data set passed to it is greater the second one
	 0 if the data sets are equal

in terms of the ordinal relationship of the keys contained in the data sets.

Example.

typedef {
	int key;
	char data [100];
} myData_t;

int compareMyData (void* pa, void* pb) {
	int ka = ((myData_t*) pa)->key;
	int kb = ((myData_t*) pb)->key;
	return (ka < kb) ? -1 : (ka > kb) ? 1 : 0;
}

All data to be inserted into the avl tree is passed to the related functions using pointers; i.e. the application 
is responsible for storing the data itself (it could e.g. allocate data memory on the heap and pass the resulting
pointer to avl_tree_insert). The avl tree manager will only deal with these pointers and pass them to any user 
supplied functions where applicable (i.e. for comparison and processing).

avl tree functions removing data from the avl tree can be told to also delete the data itself, which comes handy
if the data is not needed anymore and was allocated on the heap, in which case the application doesn't have to free
it itself. This applies to avltree_delete, avltree_replace and avltree_destroy.

To processing avl tree data, call avltree_walk. avltree_walk expects a user supplied function parameter that it 
will call for each avltree node it reaches, passing a pointer to the data stored in that node to this function. 
The function signature is:

	typedef bool (*nodeProcessor_t) (void* const);

The avl tree manager will quit traversing the avl tree as soon as the processing function returns false, and in that
case will also return false, indicating that the application decided to prematurely end avl tree traversal. This 
comes handy when you are looking for a particular data set and want to stop looking any further once you have found it.


