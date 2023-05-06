#include <list>
#include <queue>
#include <stack>

#include <bits/stdc++.h>

using namespace std;
//�Nstate����Ƶ��c�w��int* (�s���m���)
typedef int* State;

const int SIDE = 4;

bool noSolution(State state) {
	int disOrder = 0, rowContainsZero;
	for (int i = 0; i < SIDE * SIDE; i++) {
		if (state[i]) {
			for (int j = 0; j < i; j++)
				if (state[j] && state[i] < state[j]) disOrder++;
		}
		else rowContainsZero = i / SIDE;
	}
	return (disOrder + !(SIDE % 2) * rowContainsZero) % 2;
}

//�ԥX�Ӽg
//	h1
int misplaceAmount(State state) {
	int amount = 0;
	for (int i = 0; i < SIDE * SIDE; i++)
		if (state[i] != i) amount++;
	return amount;
}

//	h2
int manhattanDist(State state) {
	int dist = 0;
	for (int i = 0; i < SIDE * SIDE; i++)
		if (state[i])
			dist += abs(i % SIDE - state[i] % SIDE) + abs(i / SIDE - state[i] / SIDE);
	return dist;
}

//	h3
int outofline(State state) {
	int out = 0;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 0; j < SIDE; j++) {
			if (state[i * SIDE + j] / SIDE != i) out++;
			if (state[i * SIDE + j] % SIDE != j) out++;
		}
	}
	return out;
}

//	h4
int connectivity(State state) {
	int conn = 0, dvhc, dhrz;
	for (int i = 0; i < SIDE; i++) {
		for (int j = 1; j < SIDE; j++) {
			if (state[i * SIDE + j - 1] && state[i * SIDE + j]) {
				dvhc = abs(state[i * SIDE + j - 1] / SIDE - state[i * SIDE + j] / SIDE);
				dhrz = abs(state[i * SIDE + j - 1] % SIDE - state[i * SIDE + j] % SIDE);
				if (dvhc == 1 && dhrz == 0 || dvhc == 0 && dhrz == 1) conn++;
			}
			if (state[(j - 1) * SIDE + i] && state[j * SIDE + i]) {
				dvhc = abs(state[(j - 1) * SIDE + i] / SIDE - state[j * SIDE + i] / SIDE);
				dhrz = abs(state[(j - 1) * SIDE + i] % SIDE - state[j * SIDE + i] % SIDE);
				if (dvhc == 1 && dhrz == 0 || dvhc == 0 && dhrz == 1) conn++;
			}
		}
	}
	return 2 * SIDE * (SIDE - 1) - conn;
}

list<pair<State, int>> findNextSteps(State state) {
	list<pair<State, int>> stepList;
	int pos_0 = 0;
	while (state[pos_0]) pos_0++;
	if (pos_0 / SIDE != 0) {
		State newSt = new int[SIDE * SIDE];
		for (int i = 0; i < SIDE * SIDE; i++)
			if (i == pos_0 - SIDE) newSt[i] = 0;
			else if (i == pos_0) newSt[i] = state[pos_0 - SIDE];
			else newSt[i] = state[i];
		stepList.push_back(make_pair(newSt, 0));
	}
	if (pos_0 / SIDE != SIDE - 1) {
		State newSt = new int[SIDE * SIDE];
		for (int i = 0; i < SIDE * SIDE; i++)
			if (i == pos_0 + SIDE) newSt[i] = 0;
			else if (i == pos_0) newSt[i] = state[pos_0 + SIDE];
			else newSt[i] = state[i];
		stepList.push_back(make_pair(newSt, 1));
	}
	if (pos_0 % SIDE != 0) {
		State newSt = new int[SIDE * SIDE];
		for (int i = 0; i < SIDE * SIDE; i++)
			if (i == pos_0 - 1) newSt[i] = 0;
			else if (i == pos_0) newSt[i] = state[pos_0 - 1];
			else newSt[i] = state[i];
		stepList.push_back(make_pair(newSt, 2));
	}
	if (pos_0 % SIDE != SIDE - 1) {
		State newSt = new int[SIDE * SIDE];
		for (int i = 0; i < SIDE * SIDE; i++)
			if (i == pos_0 + 1) newSt[i] = 0;
			else if (i == pos_0) newSt[i] = state[pos_0 + 1];
			else newSt[i] = state[i];
		stepList.push_back(make_pair(newSt, 3));
	}
	return stepList;
}

bool isGoal(State state) {
	for (int i = 0; i < SIDE * SIDE; i++)
		if (state[i] != i) return false;
	return true;
}

//�Ӥw�y�X���A�P������T����`�I�A�a�����|�B�[�v��
class DataNode {

public:
	State state;
	DataNode* parent;
	int action;		//��parent�����@������ (�W�@�B���ʤ�V)
	int g, h, f;	//�ܤ�����O, heuristic func��, A*() = g + h
	int heurFunc;	//��ܭ��@��heuristic function

	//state��node
	//�b��کw�nheurFunc�A�����N���B�z�[�v
	DataNode(State state, int heurFunc = 0) {
		this->state = state;
		this->heurFunc = heurFunc;
		this->parent = NULL;
		if(heurFunc) g = 0; h = heuristic(heurFunc); f = g + h;
	}
	//�l�`�I�@�ߩ���heurFunc
	DataNode(State state, DataNode* parent, int action) {
		this->state = state;
		heurFunc = parent->heurFunc;
		this->parent = parent;
		this->action = action;
		if (heurFunc) g = parent->g + costAddition(heurFunc); h = heuristic(heurFunc); f = g + h;
	}
	
	int costAddition(int func) {
		int g = 0;
		if (func == 1)
			g = 1;
		else if (func == 2)
			g = 1;
		else if (func == 3)
			g = 1;
		else if (func == 4)
			g = 9;
		return g;
	}

	int heuristic(int func) {
		int h = 0;
		if (func == 1)
			h = misplaceAmount(state);
		else if (func == 2)
			h = manhattanDist(state);
		else if (func == 3)
			h = outofline(state);
		else if (func == 4)
			h = 9 * connectivity(state) + misplaceAmount(state);
		return h;
	}
	/* �۷� PriQueNode compare�� case2
	int costAddition(int func) {
		int g = 0;
		if (func == 1)
			g = 9;
		else if (func == 2)
			g = 23;
		else if (func == 3)
			g = 18;
		else if (func == 4)
			g = 13;
		return g;
	}

	int heuristic(int func) {
		int h = 0;
		if (func == 1)
			h = 9 * dislocationAmount(state) + dislocationAmount(state);
		else if (func == 2)
			h = 23 * manhattanDist(state) + manhattanDist(state);
		else if (func == 3)
			h = 18 * outofline(state) + outofline(state);
		else if (func == 4)
			h = 13 * connectivity(state) + connectivity(state);
		return h;
	}*/

	list<DataNode*> findSuccessor() {
		list<DataNode*> successorList;
		list<pair<State, int>> nextSteps = findNextSteps(state);
		for (auto index = nextSteps.begin(); index != nextSteps.end(); index++)
			successorList.push_back(new DataNode(index->first, this, index->second));
		return successorList;
	}

	bool isGoal() {
		for (int i = 0; i < SIDE * SIDE; i++)
			if (state[i] != i) return false;
		return true;
	}
};

//priority queue ���s���� (Heap���`�I)
class PriQueNode {

public:
	DataNode* dataNode;
	int pushOrder;	//���FFIFO

	PriQueNode(DataNode* dataNode, int pushOrder) {
		this->dataNode = dataNode;
		this->pushOrder = pushOrder;
	}

	//priority queue ���Ƨǩw�q
	struct compare {
		bool operator()(PriQueNode a, PriQueNode b) {
			return (a.dataNode->f == b.dataNode->f ? a.pushOrder > b.pushOrder : a.dataNode->f > b.dataNode->f);
			// �۷� DataNode g�Bh�� case2
			//return (a.dataNode->f == b.dataNode->f ? a.dataNode->h > b.dataNode->h : a.dataNode->f > b.dataNode->f);
			// LIFO
			//return (a.dataNode->f == b.dataNode->f ? a.pushOrder < b.pushOrder : a.dataNode->f > b.dataNode->f);
		}
	};
};

//�w�q�Usearch��k
class Search {
	int showExpandNodes;
	// IDS�Ϊ�
	int searchState[SIDE * SIDE];
	int searchDeep;
	int pos_0;

	// IDS�Ϊ�
	bool search(int level) {
		if (level > searchDeep) return 0;
		
		expandNodes++;
		//�e����ܶi��
		if (expandNodes - showExpandNodes == 100000) {
			while (showExpandNodes) {
				cout << "\b";
				showExpandNodes /= 10;
			}
			showExpandNodes = expandNodes;
			cout << showExpandNodes;
		}

		if (isGoal(searchState)) return 1;
		for (int i = 0; i <= 3; i++) {
			if (!move(i)) continue;
			if (search(level + 1)) return 1;
			move(i + (i % 2 ? -1 : 1));
		}
		return 0;
		/* �¾��v
		while(!actionStack.empty()) actionStack.pop();
		DataNode* node = new DataNode(startState);
		stack<DataNode*> nodeStack;

		nodeStack.push(node);
		while (!nodeStack.empty()) {
			node = nodeStack.top();
			nodeStack.pop();
			if (node->isGoal()) {
				//while(nodeStack.pop(), !nodeStack.empty()) delete nodeStack.top();	//�d�bstack���ܤ�
				while (showExpandNodes) {
					cout << "\b";
					showExpandNodes /= 10;
				}
				return true;
			}
			if (node->level < deep) {
				list<DataNode*> successorList = node->findSuccessor();
				for (auto index = successorList.rbegin(); index != successorList.rend(); index++) {
					nodeStack.push(*index);
					expandNodes++;
					if (expandNodes - showExpandNodes == 10000) {
						while (showExpandNodes) {
							cout << "\b";
							showExpandNodes /= 10;
						}
						showExpandNodes = expandNodes;
						cout << showExpandNodes;
					}
				}
			}
			delete node;	//��b�L�{�����U�O����Ŷ�
		}
		//�`��deep���S���@��state�O���I
		return false;*/
	}
	// IDS�Ϊ�
	bool move(int action) {
		if (action == 0 && pos_0 / SIDE != 0) { swap(searchState[pos_0], searchState[pos_0 - SIDE]); pos_0 -= SIDE; }
		else if (action == 1 && pos_0 / SIDE != SIDE - 1) { swap(searchState[pos_0], searchState[pos_0 + SIDE]); pos_0 += SIDE; }
		else if (action == 2 && pos_0 % SIDE != 0) { swap(searchState[pos_0], searchState[pos_0 - 1]); pos_0--; }
		else if (action == 3 && pos_0 % SIDE != SIDE - 1) { swap(searchState[pos_0], searchState[pos_0 + 1]); pos_0++; }
		else return 0;
		return 1;
	}

public:
	stack<int> actionStack;
	int expandNodes;	//�i�}���`�I��(enqueue����)
	
	bool IDSearch(State startState, int cutoff = -1) {
		string str = " [Current expand nodes]: ";
		cout << str;
		expandNodes = showExpandNodes = 0;
		copy_n(startState, SIDE * SIDE, searchState);
		searchDeep = 0;
		pos_0 = -1;
		while(searchState[++pos_0]);
		while (searchDeep <= cutoff && !search(0)) searchDeep++;

		while (showExpandNodes) { cout << "\b \b"; showExpandNodes /= 10; }
		for (int i = 0; i < str.length(); i++) cout << "\b \b";

		if (searchDeep > cutoff) return false;
		return true;
	}

	bool AstarSearch(State startState, int heurFunc) {
		while (!actionStack.empty()) actionStack.pop();
		expandNodes = 0;
		DataNode* node = new DataNode(startState, heurFunc);
		priority_queue<PriQueNode, vector<PriQueNode>, PriQueNode::compare> pqueue;
		int pushOrder = 0;

		pqueue.push(PriQueNode(node, ++pushOrder));
		while (!pqueue.empty()) {
			node = pqueue.top().dataNode;
			pqueue.pop();
			if (node->isGoal()) {
				//�˱����|
				while (node->parent) {
					actionStack.push(node->action);
					node = node->parent;
				}
				return true;
			}
			list<DataNode*> successorList = node->findSuccessor();
			for (auto index = successorList.begin(); index != successorList.end(); index++) {
				pqueue.push(PriQueNode(*index, ++pushOrder));
				expandNodes++;
			}
		}
		//��Ҧ��i�ೣ�����F���S���@��state�O���I (���ӥi�ਫ�����N�O�F)
		return false;
	}
};

int main() {
	State state = new int[SIDE * SIDE];
	Search search;
	string heurFuncName[4] = { "misplace amount", "Manhattan distance", "out of line", "connectivity" };
	string initial[3][20] = {	//�D�ث��w����
		{	"412386A7095BCDEF", "4123967058ABCDEF", "412390675DAB8CEF", "123745B0896ACDEF", "4123857B9A6FCDE0",
			"165349278DABC0EF", "1263457B89AFC0DE", "152384679ABFCDE0", "4612507389ABCDEF", "1276405389ABCDEF"	},
		{	"152349870A6BCDEF", "157249638D0BCEAF", "41538927AD6BC0EF", "15234967AEDB8CF0", "19524A6308B7CDEF",
			"15239D674CAB8EF0", "15234A968D07CEBF", "156249378A0BCDEF", "195284730A6BCDEF", "1523C467098BDEAF"	},
		{	"52371D964CAB80EF", "153A96204DB78CEF", "152396A7CD0B48EF", "91536D274A0B8CEF", "1523406BD97F8CAE",
			"5263107B49DA8CEF", "15239D6784BFCEA0", "15234A70D6EB89CF", "12934057DC6B8EAF", "152396A78D0BC4EF"	}
	};
	float avgExpands[3][5] = { 0 };

	// Randomizing �ۭq����
	srand(time(NULL));
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 10; i++) {
			string str = "0123456789ABCDEF";
			int pos_0 = 0, action, last = 4;
			// Dice (�u����2�B�j��)
			for (int j = 0; j < 8 + 4 * k; j++) {
				action = rand() % 4;
				if (action == last + (last % 2 ? -1 : 1)) action = -1;
				if (action == 0 && pos_0 / SIDE != 0) {
					swap(str[pos_0], str[pos_0 - SIDE]); pos_0 -= SIDE; last = action;
				}
				else if (action == 1 && pos_0 / SIDE != SIDE - 1) {
					swap(str[pos_0], str[pos_0 + SIDE]); pos_0 += SIDE; last = action;
				}
				else if (action == 2 && pos_0 % SIDE != 0) {
					swap(str[pos_0], str[pos_0 - 1]); pos_0--; last = action;
				}
				else if (action == 3 && pos_0 % SIDE != SIDE - 1) {
					swap(str[pos_0], str[pos_0 + 1]); pos_0++; last = action;
				}
				else j--;
			}
			initial[k][10 + i] = str;
			//�P�_���S�����|: �γ̧֪�heuristic�j�M�ݬO���O�b���w�`�� (�i�o�P�_�@�s��actions��states�X�{12�L�b�j��)
			for (int i = 0; i < SIDE * SIDE; i++) {
				if ('0' <= str[i] && str[i] <= '9') state[i] = str[i] - '0';
				else state[i] = 10 + str[i] - 'A';
			}
			search.AstarSearch(state, 2);
			if (search.actionStack.size() != 8 + 4 * k) i--;
		}
	}

	// Searching
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 20; i++) {
			string str = initial[k][i];
			cout << "\nGiving " << i + 1 << "th deep " << 8 + 4 * k << " initial state : " << str << "\n";
			for (int i = 0; i < SIDE * SIDE; i++) {
				if ('0' <= str[i] && str[i] <= '9') state[i] = str[i] - '0';
				else state[i] = 10 + str[i] - 'A';
			}
			if (noSolution(state)) {
				cout << "This case has no solution!!\n";
				return 0;
			}

			if (!search.IDSearch(state, 16)) cout << "- No solution in 16 depth search by Interative Deeping!!\n";
			cout << "- Total expand nodes search by Interative Deeping : " << search.expandNodes << "\n";
			avgExpands[k][0] += search.expandNodes / 20.0;

			for (int h = 1; h <= 4; h++) {
				if (!search.AstarSearch(state, h)) {
					cout << "- No solution search by " << heurFuncName[h - 1] << " Heuristic A*\n";
					continue;
				}
				cout << "- Total expand nodes search by " << heurFuncName[h - 1] << " Heuristic A* : " << search.expandNodes << "\n";
				avgExpands[k][h] += search.expandNodes / 20.0;
			}
			//cout << search.actionStack.size() << "\n";
			/*string direction[4] = {"up", "down", "left", "right"};
			//�˱����|
			while (!search.actionStack.empty()) {
				cout << "move 0 to " + direction[search.actionStack.top()] + "\n";
				search.actionStack.pop();
			}*/
		}
		cout << "\n------------------------------------------------------------------------\n";
		cout << "Average expand nodes search by IDS: " << (int)(avgExpands[k][0] + 0.5) << "\n";
		for (int h = 1; h <= 4; h++)
			cout << "Average expand nodes search by " << heurFuncName[h - 1] << ": " << (int)(avgExpands[k][h] + 0.5) << "\n";
		cout << "------------------------------------------------------------------------\n";
	}

	cout << "\n------------------------- Average expand nodes -------------------------\n\n";
	cout << "                                Deep 8    Deep 12       Deep 16\n";
	cout << "Search by " << right << setw(20) << "IDSearch :";
	for (int k = 0; k < 3; k++)
		cout << right << setw(8 + k * 3) << (int)(avgExpands[k][0] + 0.5);
	cout << "\n";
	for (int h = 1; h <= 4; h++) {
		cout << "Search by " << right << setw(20) << heurFuncName[h - 1] + " :";
		for (int k = 0; k < 3; k++)
			cout << right << setw(8 + k * 3) << (int)(avgExpands[k][h] + 0.5);
		cout << "\n";
	}
	cout << "------------------------------------------------------------------------\n";
}