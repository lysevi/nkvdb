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
	ss << "v: " << this->value << "{";
	if (this->left != nullptr)
		ss << this->left->toString() << "}";
	else
		ss << "}";

	ss << " {";
	if (this->right != nullptr)
		ss << this->right->toString() << "}";
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
		newNode->clr = Colour::RED;
		if (root == nullptr) {
			root = newNode;
			root->clr = Colour::BLACK;
			return;
		}
		rec_push(newNode, root);
	}

	void rec_push(Node*newValue, Node*cur_pos) {
		Node *nextPos = nullptr;
		if (newValue->value > cur_pos->value) {
			if (cur_pos->right == nullptr) {
				cur_pos->right = newValue;
				newValue->parent = cur_pos;
			} else {
				nextPos = cur_pos->right;
			}
		} else {
			if (cur_pos->left == nullptr) {
				cur_pos->left = newValue;
				newValue->parent = cur_pos;
			} else {
				nextPos = cur_pos->left;
			}
		}
		if (nextPos != nullptr){
			rec_push(newValue, nextPos);
		}
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