#define _CRT_SECURE_NO_WARNINGS
//cpp headers
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
//c headers
#include <stdio.h>
#include <math.h>

using namespace std;


template <typename T>
struct s_Node {
	T				data;
	bool			leaf;
	s_Node*			pparent;
	vector<s_Node*> pvchild;

	s_Node() { leaf = 0; pparent = NULL; }
	s_Node(T val, bool bleaf=0, s_Node* pp=NULL):data(val),leaf(bleaf),pparent(pp) {}
};



template <typename T>
class c_Tree {
private:
	s_Node<T>*		root;
	int				sum;

public:
	c_Tree() { root = NULL; sum = 1; }
	c_Tree(s_Node<T>* pr):root(pr), sum(1) {}
	int			get_sum(void) { return this->sum; }
	s_Node<T>*	init(s_Node<T>* pnode);
	s_Node<T>*	putChild(s_Node<T>* pnode, s_Node<T>* pparent_node);
	void	leaf(s_Node<T>* pnode) { pnode->leaf = 1; }
	void	traversal(s_Node<T>* pnode);
	void	traversal() { this->traversal(this->root); }
};

template <typename T>
s_Node<T>*	c_Tree<T>::init(s_Node<T>* pnode)
{
	s_Node<T>* pNew = new s_Node<T>;
	if (pNew != NULL) {
		*pNew = *pnode;
		this->root = pNew;
		sum = 1;
		return pNew;
	}
	return NULL;
}

template <typename T>
s_Node<T>*	c_Tree<T>::putChild(s_Node<T>* pnode, s_Node<T>* pparent_node)
{
	s_Node<T>* pNew = new s_Node<T>;
	if (pNew != NULL) {
		*pNew = *pnode;
		pNew->pparent = pparent_node;
		pparent_node->pvchild.push_back(pNew);
		this->sum++;
		return pNew;
	}
	return NULL;
}

template <typename T>
void c_Tree<T>::traversal(s_Node<T>* pnode)
{
	if (pnode == NULL) { cout << "nullptr" << endl; return; }
	cout << pnode->data;
	for (int i = 0; i < (int)pnode->pvchild.size(); i++) {
		cout << "   ";
		traversal(pnode->pvchild[i]);
	}
	return;
}



//if discrete:		sum[0] is sum of good, sum[1] is sum of bad
//if continuous:	sum[0],sum[1] are sum_val when <= div_val (-), sum[2],sum[3] are sum_val when > div_val (+)
class c_statistic {
public:
	string				attribute;
	vector<string>		vfeature;
	vector<vector<int>>	vsum;
};



/*
 * param:	pdata:		all of data read from the txt file					[row * col]
 *			ppure:		remove top row and first column from the pdata		[row-1 * col-1]
 *			psta:		compute the sum of good/bad items of each feature	[col-1 * 1]
 *			pconti_idx:	index of the continuous value columns in ppure		[row-1 * n]
 */
void prepare(string filepath, int row, int col, vector<vector<string>>* pdata, vector<vector<string>>* ppure, \
	vector<c_statistic>* psta, vector<vector<double>>* pconti_divide, vector<int>* pconti_idx)
{
	FILE* fp = NULL;
	psta->resize((size_t)col-1);
	pdata->resize(row);
	for (int i = 0; i < row; i++)
		pdata->at(i).resize(col);
	fp = fopen(filepath.c_str(), "r");
	if (fp == NULL)
		return;

	//increase attributes for continuous column like '密度0.244','密度0.294',...
	char cc = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col - 1; j++) {
			while ((cc = getc(fp)) != ',') {
				pdata->at(i)[j] += cc;
			}
			if ((i == 0) && (j != 0))
				psta->at((size_t)j - 1).attribute = pdata->at(i)[j];
			else;
		}
		while ((cc = getc(fp)) != '\n') {
			if (cc == EOF)
				break;
			pdata->at(i)[(size_t)col - 1] += cc;
		}
		if (i == 0)
			psta->at((size_t)col - 2).attribute = pdata->at(i)[(size_t)col - 1];
	}

	vector<vector<string>> pure_t(*pdata);
	pure_t.erase(pure_t.begin());					//erase top row
	for (int i = 0; i < row - 1; i++)
		pure_t[i].erase(pure_t[i].begin());			//erase first column
	ppure->swap(pure_t);							//swap ppure and pure_t

	//dispaly pdata, ppure and psta
	cout << "\n > data <" << endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++)
			cout << pdata->at(i)[j] << "\t";
		cout << endl;
	}
	cout << "\n > pure <" << endl;
	for (int i = 0; i < row - 1; i++) {
		for (int j = 0; j < col - 1; j++)
			cout << ppure->at(i)[j] << "\t";
		cout << endl;
	}
	
	fclose(fp);
	return;
}



/*
 * param:	ppure:		pointer of pure data
 *			psta:		pointer of statistics
 *			pconti_div:	div_val of the continuous value columns
 *			pconti_idx:	index of the continuous value columns
 */
void Statistics(vector<vector<string>>* ppure, vector<c_statistic>* psta, vector<vector<double>>* pconti_divide, \
	vector<int>* pconti_idx)
{

	//discretize the continuous values
	int	nconti = 0;
	vector<vector<double>> conti_values(pconti_idx->size());		//save the continuous values (string -> double -> sort)
	pconti_divide->resize(pconti_idx->size());						//save the divide value to split the conti_values
	for (int j = 0; j < (int)psta->size(); j++) {
		if (j == pconti_idx->at(nconti)) {
			conti_values[nconti].resize(ppure->size());
			pconti_divide->at(nconti).resize(ppure->size() - 1);
			for (int i = 0; i < (int)ppure->size(); i++)
				conti_values[nconti][i] = stod(ppure->at(i)[j]);
			sort(conti_values[nconti].begin(), conti_values[nconti].end());
			for (int i = 0; i < (int)ppure->size() - 1; i++)
				pconti_divide->at(nconti)[i] = (conti_values[nconti][i] + conti_values[nconti][(size_t)i + 1]) / 2.0;
			nconti++;
			if (nconti == (int)pconti_idx->size())
				nconti = (int)pconti_idx->size() - 1;
		}
		else;
	}

	//compute statistics
	nconti = 0;
	for (int j = 0; j < (int)psta->size(); j++) {
		if (j == pconti_idx->at(nconti)) {									//when meet continuous columns
			char c_str[10] = { 0 };
			psta->at(j).vfeature.resize(pconti_divide->at(nconti).size(), "");
			psta->at(j).vsum.resize(pconti_divide->at(nconti).size());
			for (int n = 0; n < psta->at(j).vsum.size(); n++)
				psta->at(j).vsum[n].resize(4, 0);							//vsum[0]/[1] = (val<=div), vsum[2]/[4] = (val>div)
			for (int k = 0; k < psta->at(j).vsum.size(); k++) {				//create the <c_string> features of divide values
				int ccc = sprintf(c_str, "%6.4f", pconti_divide->at(nconti)[k]);
				psta->at(j).vfeature[k] = c_str;
			}

			for (int i = 0; i < ppure->size(); i++) {						//loop of every row of continuous value
				int t = 0;
				for (t = 0; t < (int)psta->at(j).vfeature.size(); t++) {	//loop of every divide value
					if (stod(ppure->at(i)[pconti_idx->at(nconti)]) <= pconti_divide->at(nconti)[t]) {
						if (ppure->at(i)[psta->size() - 1] == "是")				psta->at(j).vsum[t][0]++;
						else if (ppure->at(i)[psta->size() - 1] == "否")		psta->at(j).vsum[t][1]++;
						else { cout << "\n***** 最后一列没有发现有效的标签 *****\n"; return; }
					}
					else {
						if (ppure->at(i)[psta->size() - 1] == "是")				psta->at(j).vsum[t][2]++;
						else if (ppure->at(i)[psta->size() - 1] == "否")		psta->at(j).vsum[t][3]++;
						else { cout << "\n***** 最后一列没有发现有效的标签 *****\n"; return; }
					}
				}
			}
			nconti++;
			if (nconti == (int)pconti_idx->size())
				nconti = (int)pconti_idx->size() - 1;
		}
		else {
			for (int i = 0; i < ppure->size(); i++) {
				int t = 0;
				for (t = 0; t < (int)psta->at(j).vfeature.size(); t++) {
					if (ppure->at(i)[j] == psta->at(j).vfeature[t]) {
						if (ppure->at(i)[psta->size() - 1] == "是")				psta->at(j).vsum[t][0]++;
						else if (ppure->at(i)[psta->size() - 1] == "否")		psta->at(j).vsum[t][1]++;
						else { cout << "\n***** 最后一列没有发现有效的标签 *****\n"; return; }
						break;
					}
					else;
				}
				if (t == psta->at(j).vfeature.size()) {
					string newfeature = ppure->at(i)[j];
					vector<int> newsum(2, 0);
					if (ppure->at(i)[psta->size() - 1] == "是")					newsum[0] = 1;
					else if (ppure->at(i)[psta->size() - 1] == "否")			newsum[1] = 1;
					else { cout << "\n***** 最后一列没有发现有效的标签 *****\n"; return; }
					psta->at(j).vfeature.push_back(newfeature);
					psta->at(j).vsum.push_back(newsum);
				}
				else;
			}
		}
	}

	//display
	cout << "\n > attr <" << endl;
	nconti = 0;
	for (int i = 0; i < (int)psta->size(); i++) {
		cout << psta->at(i).attribute << ":";
		if (i == pconti_idx->at(nconti)) {
			for (int j = 0; j < (int)psta->at(i).vfeature.size(); j++) {
				cout << "\t(" << psta->at(i).vfeature[j] << ")[" << psta->at(i).vsum[j][0] << "/" << psta->at(i).vsum[j][1] \
					<< "/" << psta->at(i).vsum[j][2] << "/" << psta->at(i).vsum[j][3] << "]";
				if (j == ((int)psta->at(i).vfeature.size() - 1) || (((j + 1) % 4 == 0) && (j != 0)))
					cout << endl;
			}
			nconti++;
			if (nconti == (int)pconti_idx->size())
				nconti = (int)pconti_idx->size() - 1;
		}
		else {
			for (int j = 0; j < (int)psta->at(i).vfeature.size(); j++) {
				cout << "(" << psta->at(i).vfeature[j] << ")[";
				cout << psta->at(i).vsum[j][0] << "/" << psta->at(i).vsum[j][1];
				cout << "]  ";
			}cout << endl;
		}
	}
	return;
}



/*
 * param:	ppure:		pointer of pure data
 *			psta:		pointer of statistics
 *			pconti_div:	div_val of the continuous value columns
 *			pconti_idx:	index of the continuous value columns
 *			pidx:		pointer of all max_idx (max_att_idx, max_conti_1_idx, max_conti_2_idx, ...)
 */
void Gain_InfoEntropy(vector<size_t>* pidx, vector<vector<string>>* ppure, vector<c_statistic>* psta, vector<vector<double>>* pconti_divide, \
	vector<int>* pconti_idx, bool b_show)
{
	int		nconti = 0;
	int		posi = 0, nega = 0;
	double	p1 = 0.0, p2 = 0.0;
	double	origin_entropy = 0.0;

	//compute original entropy
	posi = psta->at(psta->size() - 1).vsum[0][0];
	nega = psta->at(psta->size() - 1).vsum[1][1];
	p1 = (double)posi / ppure->size();
	p2 = (double)nega / ppure->size();
	origin_entropy = -(p1 * log2(p1) + p2 * log2(p2));
	if (b_show)
		cout << "\n > Dataset Entropy <\norigin_entropy = " << origin_entropy << endl;

	//compute attribute entropy
	vector<vector<double>>	att_entropy_history(psta->size() - 1);
	vector<double>			att_entropy(psta->size() - 1, 0.0);
	vector<vector<double>>	conti_att_entropy(pconti_idx->size());			//exclusive vec_entropy for continuous value
	vector<vector<double>>	weight(psta->size() - 1);
	vector<double>			gain(psta->size() - 1, 0.0);
	vector<vector<double>>	conti_gain(pconti_idx->size());					//exclusive vec_gain for continuous value
	vector<int>				conti_max_idx(pconti_idx->size(), 0);			//save the max_val's [idx] in each continuoue columns
	
	nconti = 0;
	for (int i = 0; i < (int)psta->size() - 1; i++)							//loop of every attribute EXCEPT column[good/bad]
	{
		if (i == pconti_idx->at(nconti)) {
			conti_att_entropy[nconti].resize(psta->at(i).vfeature.size(), 0.0);
			conti_gain[nconti].resize(psta->at(i).vfeature.size(), 0.0);
			for (int j = 0; j < (int)psta->at(i).vfeature.size(); j++)		//loop of every feature
			{
				int		posi1 = psta->at(i).vsum[j][0];						// <=,good
				int		nega1 = psta->at(i).vsum[j][1];						// <=,bad
				int		posi2 = psta->at(i).vsum[j][2];						// >,good
				int		nega2 = psta->at(i).vsum[j][3];						// >bad
				int		lesse = posi1 + nega1;								// less or equal
				int		biger = posi2 + nega2;
				double	p11 = (double)posi1 / lesse;						//guarantee that p(n)(n) will never eauql to 0
				double	p12 = (double)nega1 / lesse;
				double	p21 = (double)posi2 / biger;
				double	p22 = (double)nega2 / biger;
				double	lesse_entropy = 0.0;
				double	biger_entropy = 0.0;

				if ((p11 == 0) && (p12 != 0))			lesse_entropy = -(p12 * log2(p12));
				else if ((p11 != 0) && (p12 == 0))		lesse_entropy = -(p11 * log2(p11));
				else if ((p11 == 0) && (p12 == 0))		lesse_entropy = 0.0;
				else									lesse_entropy = -(p11 * log2(p11) + p12 * log2(p12));

				if ((p21 == 0) && (p22 != 0))			biger_entropy = -(p22 * log2(p22));
				else if ((p21 != 0) && (p22 == 0))		biger_entropy = -(p21 * log2(p21));
				else if ((p21 == 0) && (p22 == 0))		biger_entropy = 0.0;
				else									biger_entropy = -(p21 * log2(p21) + p22 * log2(p22));

				double	more_weight_t = (double)biger / ppure->size();		//lesse_item's weight = (1 - more_weight)

				conti_att_entropy[nconti][j] = (1 - more_weight_t) * lesse_entropy + more_weight_t * biger_entropy;
				conti_gain[nconti][j] = origin_entropy - conti_att_entropy[nconti][j];
			}

			vector<double>::iterator it1 = max_element(conti_att_entropy[nconti].begin(), conti_att_entropy[nconti].end());
			vector<double>::iterator it2 = max_element(conti_gain[nconti].begin(), conti_gain[nconti].end());
			att_entropy[i] = *it1;
			gain[i] = *it2;
			conti_max_idx[nconti] = (int)(it2 - conti_gain[nconti].begin());

			nconti++;
			if (nconti == (int)pconti_idx->size())
				nconti = (int)pconti_idx->size() - 1;
		}
		else {
			weight[i].resize(psta->at(i).vfeature.size(), 0.0);
			att_entropy_history[i].resize(psta->at(i).vfeature.size(), 0.0);
			for (int j = 0; j < (int)psta->at(i).vfeature.size(); j++)		//loop of every feature
			{
				posi = psta->at(i).vsum[j][0];
				nega = psta->at(i).vsum[j][1];
				p1 = (double)posi / ((double)posi + nega);
				p2 = (double)nega / ((double)posi + nega);
				if ((posi == 0) && (nega != 0))			att_entropy_history[i][j] = -(p2 * log2(p2));
				else if ((posi != 0) && (nega == 0))	att_entropy_history[i][j] = -(p1 * log2(p1));
				else if ((posi == 0) && (nega == 0))	att_entropy_history[i][j] = 0.0;
				else									att_entropy_history[i][j] = -(p1 * log2(p1) + p2 * log2(p2));
				weight[i][j] = (double)((double)posi + nega) / ppure->size();
				att_entropy[i] += weight[i][j] * att_entropy_history[i][j];
			}
			gain[i] = origin_entropy - att_entropy[i];
		}
	}

	//display
	if (b_show) {
		cout << "\n > Attribute Entropy <" << endl;
		nconti = 0;
		for (int i = 0; i < (int)psta->size() - 1; i++) {
			if (i == pconti_idx->at(nconti)) {
				cout << psta->at(i).attribute << ":\t" << att_entropy[i] << endl;
				for (int j = 0; j < conti_att_entropy[nconti].size(); j++) {
					cout << "\t(" << psta->at(i).vfeature[j] << ")="; printf("%9.7f", conti_att_entropy[nconti][j]);
					if (j == ((int)psta->at(i).vfeature.size() - 1) || (((j + 1) % 4 == 0) && (j != 0)))
						cout << endl;
				}
				nconti++;
				if (nconti == (int)pconti_idx->size())
					nconti = (int)pconti_idx->size() - 1;
			}
			else
				cout << psta->at(i).attribute << " = " << att_entropy[i] << endl;
		}
		cout << "\n > Gain <" << endl;
		nconti = 0;
		for (int i = 0; i < (int)psta->size() - 1; i++) {
			if (i == pconti_idx->at(nconti)) {
				cout << psta->at(i).attribute << ":\t" << gain[i] << endl;
				for (int j = 0; j < conti_att_entropy[nconti].size(); j++) {
					cout << "\t(" << psta->at(i).vfeature[j] << ")="; printf("%9.7f", conti_gain[nconti][j]);
					if (j == ((int)psta->at(i).vfeature.size() - 1) || (((j + 1) % 4 == 0) && (j != 0)))
						cout << endl;
				}
				nconti++;
				if (nconti == (int)pconti_idx->size())
					nconti = (int)pconti_idx->size() - 1;
			}
			else
				cout << psta->at(i).attribute << " = " << gain[i] << endl;
		}
	}

	//choose the [attribute] whose gain is the max to be deleted
	bool can_del = 1;
	vector<double>::iterator it = max_element(gain.begin(), gain.end());
	int max_idx = int(it - gain.begin());
	cout << "最大的Gain: (" << psta->at(max_idx).attribute;
	for (int i = 0; i < pconti_idx->size(); i++) {
		if (pconti_idx->at(i) == max_idx) {
			can_del = 0;													//indicates that conti_col can't be deleted
			cout << "->" << psta->at(max_idx).vfeature[conti_max_idx[i]];
			break;
		}
	}
	cout << ") = " << gain[max_idx] << endl;

	pidx->resize(0);
	pidx->push_back(max_idx);
	pidx->insert(pidx->begin() + 1, conti_max_idx.begin(), conti_max_idx.end());
	
	return;
}



/*
 * param:	pconti_idx:	pointer of conti_col_idx 
 *			del_idx:	the col idx that will be deleted
 * note:	----		if del_idx > conti_idx, then conti_idx doesn't need to be corrected
 */
void Correct_conti_idx(vector<int>* pconti_idx, int del_idx)
{
	int flag_correct = 1;
	for (int i = 0; i < pconti_idx->size(); i++) {
		if (pconti_idx->at(i) == del_idx) {
			flag_correct = 0;													//indicates that conti_col can't be corrected
			break;
		}
	}

	//correct the conti_idx
	if (flag_correct) {
		int p = 0;
		for (p = 0; p < pconti_idx->size(); p++) {
			if (pconti_idx->at(p) == del_idx)
				cout << "\n***** 企图删除连续值的结点数据 *****\n";
			else if (pconti_idx->at(p) > del_idx) {
				pconti_idx->at(p) -= 1;
			}
			else;
		}
	}
	else;

	return;
}



/*
 * intro:	check whether all data have same features in every attribute or not
 * return:	rtn 0 if false, 1 if good is more, 2 if bad is more, 3 if equal
 */
int Check_Recursion_Case2(vector<vector<string>>* ppure, vector<c_statistic>* psta)
{
	for (int i = 0; i < psta->size() - 1; i++) {
		for (int j = 0; j < psta->at(i).vfeature.size(); j++) {
			if ((size_t)psta->at(i).vsum[j][0] + psta->at(i).vsum[j][1] != ppure->size() || (size_t)psta->at(i).vsum[j][0] + psta->at(i).vsum[j][1] != 0)
				return 0;
			else;
		}
	}

	if (psta->at(psta->size() - 1).vsum[0][0] > psta->at(psta->size() - 1).vsum[1][1])		//if good > bad
		return 1;
	else if (psta->at(psta->size() - 1).vsum[1][1] < psta->at(psta->size() - 1).vsum[0][0])	//if good < bad
		return 2;
	else {
		cout << "\nCase2 所有样本数据 在所有属性上 的取值相同，应该选择最多的类别，但是好坏参半" << endl;
		return 3;
	}
}


/*
 * intro:	----	Recursion TreeGeneration
 * param:	branch:	branch name of the father_node to arrive the next child_node
 */
void TreeGenerate(c_Tree<string>* ptree, s_Node<string>* pparent_node, vector<vector<string>>* ppure_part, \
	vector<c_statistic>* psta_part, vector<vector<double>>* pconti_divide, vector<int>* pconti_idx, string branch)
{
	cout << "\n\n >======================== Tree Generate Recursion ========================<\n";

	s_Node<string>	node("");
	s_Node<string>* pnode_DT;

	//compute the statistics to vec_sta
	Statistics(ppure_part, psta_part, pconti_divide, pconti_idx);

	//End of recursion --- (case 1)
	if (psta_part->at(psta_part->size() - 1).vsum[0][0] == ppure_part->size()) {			//if all are good
		cout << "[都是好瓜.]" << endl;
		node.data = "if" + branch + ",好瓜"; node.leaf = 1;
		if (pparent_node == NULL)
			ptree->init(&node);
		else
			ptree->putChild(&node, pparent_node);
		return;
	}
	else if (psta_part->at(psta_part->size() - 1).vsum[1][1] == ppure_part->size()) {		//if all are bad
		cout << "[都是坏瓜.]" << endl;
		node.data = "if" + branch + ",坏瓜"; node.leaf = 1;
		if (pparent_node == NULL)
			ptree->init(&node);
		else
			ptree->putChild(&node, pparent_node);
		return;
	}
	else;

	//End of recursion --- (case 2)
	int ck = Check_Recursion_Case2(ppure_part, psta_part);									//if all data have same features
	if (psta_part->size() == 1 || ck > 0 ) {
		if (ck == 1) {
			node.data = "if" + branch + ",好瓜"; node.leaf = 1;
			if (pparent_node == NULL)
				ptree->init(&node);
			else
				ptree->putChild(&node, pparent_node);
			return;
		}
		else if (ck == 3) {
			node.data = "if" + branch + ",中瓜"; node.leaf = 1;
			if (pparent_node == NULL)
				ptree->init(&node);
			else
				ptree->putChild(&node, pparent_node);
			return;
		}
		else if (ck == 2) {
			node.data = "if" + branch + ",坏瓜"; node.leaf = 1;
			if (pparent_node == NULL)
				ptree->init(&node);
			else
				ptree->putChild(&node, pparent_node);
			return;
		}
		else;
	}

	//find the optimal attribute
	vector<size_t> idx;																		//all max idx
	Gain_InfoEntropy(&idx, ppure_part, psta_part, pconti_divide, pconti_idx, 1);

	//label this attribute_node
	if(branch == "")		node.data = psta_part->at(idx[0]).attribute;
	else					node.data = "if" + branch + "," + psta_part->at(idx[0]).attribute;
	size_t i;
	for (i = 0; i < pconti_idx->size(); i++) {
		if (idx[0] == pconti_idx->at(i)) {
			node.data = node.data + "(" + psta_part->at(idx[0]).vfeature[idx[i + 1]] + ")";	//如果是连续值还要追加div值
			break;
		}
	}node.data += "?";

	//put optimal_node to the tree
	if (pparent_node == NULL)		pnode_DT = ptree->init(&node);
	else							pnode_DT = ptree->putChild(&node, pparent_node);

	//====================================================== 离散 ========================================================================
	if (i == pconti_idx->size()) {															//如果最优化分属性是 离散值
		for (int r = 0; r < psta_part->at(idx[0]).vfeature.size(); r++)
		{
			//End of recursion --- (case 3)
			if (psta_part->at(idx[0]).vsum[r][0] + psta_part->at(idx[0]).vsum[r][1] == 0) {	//if this feature has no member
				if (psta_part->at(psta_part->size() - 1).vsum[0][0] > psta_part->at(psta_part->size() - 1).vsum[1][1]) {
					s_Node<string> child("好瓜"); child.leaf = 1;							//create a child_node
					ptree->putChild(&child, pnode_DT);
					continue;
				}
				else if (psta_part->at(psta_part->size() - 1).vsum[0][0] < psta_part->at(psta_part->size() - 1).vsum[1][1]) {
					s_Node<string> child("坏瓜"); child.leaf = 1;							//create a child_node
					ptree->putChild(&child, pnode_DT);
					continue;
				}
				else
					cout << "\nCase3-1 所有样本数据 在所有属性上 的取值相同，应该选择最多的类别，但是好坏参半" << endl;
			}
			else {	//下面的代码才是ID3决策树大多数时候会执行的部分 (上面都是一些退出递归的情况)
				//split pure to pure_t
				vector<vector<string>> pure_part_t;
				size_t row = 0;
				pure_part_t.resize((size_t)psta_part->at(idx[0]).vsum[r][0] + psta_part->at(idx[0]).vsum[r][1]);
				for (int ii = 0; ii < ppure_part->size(); ii++) {
					if (ppure_part->at(ii)[idx[0]] == psta_part->at(idx[0]).vfeature[r]) {
						pure_part_t[row].insert(pure_part_t[row].begin(), ppure_part->at(ii).begin(), ppure_part->at(ii).end());
						pure_part_t[row].erase(pure_part_t[row].begin() + idx[0]);
						row++;
					}
				}

				//clear sta_part as sta_part_t
				vector<c_statistic> sta_part_t(*psta_part);									//copy a backup
				for (int m = 0; m < sta_part_t.size(); m++) {								//清空统计数据的表单和数据 (不删标签)
					if (m == sta_part_t.size() - 1) {
						sta_part_t[m].vsum[0][0] = 0;
						sta_part_t[m].vsum[1][1] = 0;
					}
					else {
						sta_part_t[m].vfeature.resize(0);
						sta_part_t[m].vsum.resize(0);
					}
				}
				sta_part_t.erase(sta_part_t.begin() + idx[0]);								//delete the max_gain column

				vector<vector<double>>	conti_divide_t;
				vector<int>				conti_idx_t(*pconti_idx);
				string					branch_t = psta_part->at(idx[0]).vfeature[r];
				Correct_conti_idx(&conti_idx_t, (int)idx[0]);								//纠正conti_idx
				TreeGenerate(ptree, pnode_DT, &pure_part_t, &sta_part_t, &conti_divide_t, &conti_idx_t, branch_t);
			}
		}
	}//===================================================== 连续 ========================================================================
	else {																					//如果最优化分属性是 连续值
		for (int r = 0; r < 2; r++) {														// <= and >
			//End of recursion --- (case 3)
			if (psta_part->at(idx[0]).vsum[idx[i + 1]][(size_t)r * 2] + \
				psta_part->at(idx[0]).vsum[idx[i + 1]][(size_t)r * 2 + 1] == 0) {			//if this feature has no member
				if (psta_part->at(psta_part->size() - 1).vsum[0][0] > psta_part->at(psta_part->size() - 1).vsum[1][1]) {
					s_Node<string> child("好瓜"); child.leaf = 1;							//create a child_node
					ptree->putChild(&child, pnode_DT);
					continue;
				}
				else if (psta_part->at(psta_part->size() - 1).vsum[0][0] < psta_part->at(psta_part->size() - 1).vsum[1][1]) {
					s_Node<string> child("坏瓜"); child.leaf = 1;							//create a child_node
					ptree->putChild(&child, pnode_DT);
					continue;
				}
				else
					cout << "\nCase3-2 所有样本数据 在所有属性上 的取值相同，应该选择最多的类别，但是好坏参半" << endl;
			}
			else {	//下面的代码才是ID3决策树大多数时候会执行的部分 (上面都是一些退出递归的情况)
				//split pure to pure_t
				vector<vector<string>> pure_part_t;
				pure_part_t.resize((size_t)psta_part->at(idx[0]).vsum[idx[i + 1]][(size_t)r * 2] + \
					psta_part->at(idx[0]).vsum[idx[i + 1]][(size_t)r * 2 + 1]);

				if (r == 0) {																//if less than or equal to div_val
					size_t row = 0;
					for (int ii = 0; ii < ppure_part->size(); ii++) {
						if (stod(ppure_part->at(ii)[idx[0]]) <= pconti_divide->at(i)[idx[i + 1]]) {
							pure_part_t[row].insert(pure_part_t[row].begin(), ppure_part->at(ii).begin(), ppure_part->at(ii).end());
							row++;
						}
					}
				}
				else {																		//if greater than div_val
					size_t row = 0;
					for (int ii = 0; ii < ppure_part->size(); ii++) {
						if (stod(ppure_part->at(ii)[idx[0]]) > pconti_divide->at(i)[idx[i + 1]]) {
							pure_part_t[row].insert(pure_part_t[row].begin(), ppure_part->at(ii).begin(), ppure_part->at(ii).end());
							row++;
						}
					}
				}

				//clear sta_part as sta_part_t
				vector<c_statistic> sta_part_t(*psta_part);									//copy a backup
				for (int m = 0; m < sta_part_t.size(); m++) {								//清空统计数据的表单和数据 (不删标签)
					if (m == sta_part_t.size() - 1) {
						sta_part_t[m].vsum[0][0] = 0;
						sta_part_t[m].vsum[1][1] = 0;
					}
					else {
						sta_part_t[m].vfeature.resize(0);
						sta_part_t[m].vsum.resize(0);
					}
				}

				vector<vector<double>>	conti_divide_t;
				vector<int>				conti_idx_t(*pconti_idx);
				string					branch_t;
				if (r == 0)				branch_t = "<";
				else					branch_t = ">";
				Correct_conti_idx(&conti_idx_t, (int)idx[0]);								//纠正conti_idx
				TreeGenerate(ptree, pnode_DT, &pure_part_t, &sta_part_t, &conti_divide_t, &conti_idx_t, branch_t);
			}
		}
	}
}



int main(void)
{
	string path = "D:\\VisualStudio\\2019\\Workplace\\CppTest\\Decision_Tree\\watermelon_DT.txt";
	vector<vector<string>>	data, pure;
	vector<c_statistic>		sta;
	vector<int>				conti_idx = { 6, 7 };	//indicate the index of continuous columns in pure
	//vector<vector<double>>	conti_values;			//save the continuous values (string -> double -> sort)
	vector<vector<double>>	conti_divide;			//save the divide value to split the conti_values (n-1)

	c_Tree<string> tree;

	prepare(path, 18, 10, &data, &pure, &sta, &conti_divide, &conti_idx);
	TreeGenerate(&tree, NULL, &pure, &sta, &conti_divide, &conti_idx, "");

	cout << "\n\n >========================= std output the result =========================<\n";
	cout << " >Congratulate!<\n >Tree Info<" << endl;
	printf(" len of tree is %d\n ", tree.get_sum());
	tree.traversal();
	cout << endl;

	return 0;
}
