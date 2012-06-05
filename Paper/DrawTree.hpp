//
//  DrawTree.hpp
//  FireOnPaper
//
//  Created by Han John on 12-5-31.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef FireOnPaper_DrawTree_hpp
#define FireOnPaper_DrawTree_hpp

#include <iostream>
#include <list>
#include <set>
#include <set>
using namespace std;

class DrawTreeNode
{
private:
	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;
	DrawTreeNode* lson;
	DrawTreeNode* rson;
	DrawTreeNode* father;
	bool state;
public:
	DrawTreeNode(int x1, int y1, int x2, int y2, DrawTreeNode *father)
	{
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
		this->father = father;
		this->height = y2 - y1 + 1;
		this->width = x2 - x1 + 1;
		this->state = 0;
		lson = NULL;
		rson = NULL;
	}
    
	bool partition(set<DrawTreeNode*> &leafNodeSet)
	{
		//cout << "partition" << endl;
		if (width == 1 && height == 1)
			return false;
		int pwidth = -1;
		int pheight = -1;
		if (width >= height)
		{
			if(width % 2 ==0)
				pwidth = width / 2;
			else
				pwidth = (width + 1) / 2;
		}
		else
		{
			if (height % 2 == 0)
				pheight = height / 2;
			else
				pheight = (height + 1) / 2;
		}
        
		if (pwidth > 0)
		{
			this->lson = new DrawTreeNode(x1, y1, x1+pwidth-1, y2, this);
			this->rson = new DrawTreeNode(x1+pwidth, y1, x2, y2, this);
		}
		else if (pheight > 0)
		{
			this->lson = new DrawTreeNode(x1, y1, x2, y1+pheight-1, this);
			this->rson = new DrawTreeNode(x1, y1 + pheight, x2, y2, this);
		}
		leafNodeSet.erase(this);
		leafNodeSet.insert(this->lson);
		leafNodeSet.insert(this->rson);
	}
    
	//TODO: deside the state by their son nodes.
	DrawTreeNode* combine(set<DrawTreeNode*> &leafNodeList)
	{
		//cout << "combine" << endl;
		if(this->lson != NULL)
		{
			leafNodeList.erase(this->lson);
			free(this->lson);
			this->lson = NULL;
		}
		if (this->rson != NULL)
		{
			leafNodeList.erase(this->rson);
			free(this->rson);
			this->rson = NULL;
		}
		this->state = 1;
		return this;
	}
	bool contains(int tx, int ty)
	{
		if (x1 <= tx && x2 >= tx && y1 <= ty && y2 >= ty)
			return true;
		else
			return false;
	}
    
	void setState(bool state)
	{
		this->state = state;
	}
	DrawTreeNode* getLSon()
	{
		return lson;
	}
	DrawTreeNode* getRSon()
	{
		return rson;
	}
	DrawTreeNode* getFather()
	{
		return father;
	}
	int getX1()
	{
		return x1;
	}
	int getX2()
	{
		return x2;
	}
	int getY1()
	{
		return y1;
	}
	int getY2()
	{
		return y2;
	}
	int getWidth()
	{
		return width;
	}
	int getHeight()
	{
		return height;
	}
	int getState()
	{
		return state;
	}
	bool isLeaf()
	{
		if (lson == NULL && rson == NULL)
			return true;
		else 
			return false;
	}
	void print()
	{
		cout << "(" << x1 << "," << y1 << ", " << x2 << "," << y2 << ")"; 
	}
};



class DrawTree 
{
private:
	DrawTreeNode* root;
	set<DrawTreeNode*> leafNodeSet;
    
public:
	DrawTree(int x1, int y1, int x2, int y2)
	{
		root = new DrawTreeNode(x1, y1, x2, y2, NULL);
		leafNodeSet.insert(root);
	}
	set<DrawTreeNode*> change(int x, int y)
	{
		DrawTreeNode *tpNode = root;
		
		while(tpNode->isLeaf() == false)
		{
			
			if (tpNode->getLSon()->contains(x,y))
				tpNode = tpNode->getLSon();
			else if (tpNode->getRSon()->contains(x,y))
				tpNode = tpNode->getRSon();
		}
        
		//partition
		while(!(tpNode->getWidth() == 1 && tpNode->getHeight() == 1))
		{
			tpNode->partition(leafNodeSet);
			if (tpNode->getLSon()->contains(x,y))
				tpNode = tpNode->getLSon();
			else if (tpNode->getRSon()->contains(x,y))
				tpNode = tpNode->getRSon();
		}
		tpNode->setState(1);
		leafNodeSet.erase(tpNode);
        
		while( tpNode->getFather() != NULL && tpNode->getFather()->getLSon()->getState() == 1 && tpNode->getFather()->getRSon()->getState() == 1 && 
              tpNode->getFather()->getLSon()->isLeaf() == true && tpNode->getFather()->getRSon()->isLeaf() == true )
			tpNode = tpNode->getFather()->combine(leafNodeSet);
        
		return leafNodeSet;
	}
    
	set<DrawTreeNode*> getLeafNodeSet()
	{
		return leafNodeSet;
	}
    
};

#endif
