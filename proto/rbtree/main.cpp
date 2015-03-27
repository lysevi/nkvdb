#include <iostream>
#include <string>
#include <sstream>

using std::cout;
using std::endl;

enum class Colour
{
	RED,
	BLACK
};

struct Node
{
	int    value;
	Colour clr;
	Node   *left;
	Node   *right;
	Node() {
		left = right = nullptr;
	}
	std::string toString();
};

std::string Node::toString() {
	std::stringstream ss;
	ss << "v: " << this->value<<"{";
	if (this->left != nullptr)
		ss << this->left->toString()<<"}";
	else
		ss << "}";

	ss << " {";
	if (this->right != nullptr)
		ss << this->right->toString()<<"}";
	else
		ss << "}";

	return ss.str();
}

class RBTree
{
public:
	Node*root;
	RBTree() {
		root = nullptr;
	}
	void print() {
		if (root == nullptr) {
			cout << "null" << endl;
		} else {
			cout << root->toString();
		}
	}

	void push_value(int value) {
		Node* newNode = new Node;
		newNode->value = value;
		if (root == nullptr) {
			root = newNode;
			return;
		}
		
		rec_push(newNode, root);
	}

	void rec_push(Node*newValue, Node*cur_pos) {
		
	}
};

int main(int argc,char **argv){
	RBTree tree;
	tree.print();

	tree.push_value(0);
	tree.print();
	getchar();
}