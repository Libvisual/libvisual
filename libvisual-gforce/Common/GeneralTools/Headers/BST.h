#ifndef _BST_
#define _BST_

class BST {

	friend class BST;
	
	protected:
		BST*						mLeft;
		BST*						mRight;
		BST*						mParent;
		
		long						mKey;
		
	public:
									BST();
		virtual						~BST();


		void						Detach();
		
		BST*						Fetch( long inKey );
		
		void						Insert( BST* inNode );
		
};

#endif
