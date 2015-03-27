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
	Node   *parent;
	Node() {
		left = right = parent = nullptr;
	}
	std::string toString();
};

std::string Node::toString() {
	std::stringstream ss;
	ss << (this->clr == Colour::BLACK ? 'B' : 'R') << ": " << this->value;
	if (this->left != nullptr)
		ss << "{" << this->left->toString() << "}";
	else
		ss << "{}";


	if (this->right != nullptr)
		ss << " {" << this->right->toString() << "}";
	else
		ss << "{}";

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
		newNode->clr = Colour::RED;
		if (root == nullptr) {
			root = newNode;
			root->clr = Colour::BLACK;
			return;
		} else {
			Node *y = nullptr;
			Node *x = root;

			// Follow standard BST insert steps to first insert the node
			while (x != NULL) {
				y = x;
				if (newNode->value < x->value)
					x = x->left;
				else
					x = x->right;
			}
			newNode->parent = y;
			if (newNode->value > y->value)
				y->right = newNode;
			else
				y->left = newNode;
			newNode->clr = Colour::RED;
		}

		if (newNode->parent == nullptr)// is a root
			return;
	}
};

int main(int argc, char **argv) {
	RBTree tree;
	tree.print();

	tree.push_value(6);
	tree.push_value(5);
	tree.push_value(7);
	tree.push_value(4);
	tree.push_value(3);
	tree.push_value(1);
	tree.print();
	getchar();
}