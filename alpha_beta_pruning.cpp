#include <bits/stdc++.h>
using namespace std;

/*---- testing usage ----

int value[6] = { -999, -999, -999, -999, -999, -999 };
int decisions[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
//設置測試目標
int target[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

bool isTarget() {
	for (int i = 0; i < 8; i++)
		if (decisions[i] != target[i]) return 0;
	return 1;
}
-----------------------*/

long long int visitNodes;
bool pruning = 1;

//供 MinimaxSearch所用，因為是 minimax tree節點，所以會儲存action的拜訪進度
class State {

	static int decisionPlayer;
	static int deep;
	static int pass_flag;
	static void (*makeActionQueue)(int[], State*);

	int level;
	int actionQueue[6];
	int actionQueue_id = -1;

	int alpha;	//目前搜尋到的decision player自己目前可行走法中可保證的utility
	int beta;	//目前搜尋到的decision player對手目前可行走法中可保證的utility

	bool relocate(int pick) {
		int player = pick / 7, extra_move = 0, seeds = house[pick], opponentStore = !player * 7 + 6;
		house[pick] = 0;
		int sown = 0, index = pick;
		while (sown < seeds) {
			if (index == 13) index = 0;
			else index++;
			if (index != opponentStore) {
				house[index]++;
				sown++;
			}
		}
		if (index == player * 7 + 6) extra_move = 1;
		else if (index / 7 == player && house[index] == seeds / 13 + 1 && house[12 - index]) {
			house[player * 7 + 6] = house[player * 7 + 6] + house[index] + house[12 - index];
			house[index] = house[12 - index] = 0;
		}
		return extra_move;
	}

	//若已找完所有分支 則回傳NULL
	State* findNextSubNode() {
		while (++actionQueue_id < 6 && !house[actionQueue[actionQueue_id]]);
		if (actionQueue_id == 6) return NULL;
		visitNodes++;
		//decisions[level] = actionQueue[actionQueue_id];//t
		return new State(this, actionQueue[actionQueue_id]);
	}

	int utility() {
		return house[decisionPlayer * 7 + 6] - house[!decisionPlayer * 7 + 6];
	}

	int terminal_utility() {
		int point, store[2] = { house[6], house[13] };
		for (int i = 0; i < 6; i++) {
			store[0] += house[i];
			store[1] += house[7 + i];
		}
		return store[decisionPlayer] - store[!decisionPlayer];
	}

	bool terminal_test() {
		return !(house[0] || house[1] || house[2] || house[3] || house[4] || house[5]) ||
			!(house[7] || house[8] || house[9] || house[10] || house[11] || house[12]);
	}

	int minValue() {
		if (terminal_test()) return terminal_utility();
		if (level >= deep) return utility();
		//state有successor才會進到以下
		//if (isTarget()) print();//t
		State* nextState = findNextSubNode();
		int bestv = INT32_MAX, nextv;
		while (nextState != NULL) {
			if (pass_flag) {
				pass_flag = 0;
				nextv = nextState->minValue();
			}
			else nextv = nextState->maxValue();
			//decisions[level] = -1;//t
			//if (isTarget()) value[actionQueue[actionQueue_id] - player * 7] = nextv;//t
			if (nextv < bestv) {
				bestv = nextv;
				if (pruning && bestv <= alpha) {
					delete nextState;
					return bestv;
				}
				if (bestv < beta) beta = bestv;
			}
			State* preState = nextState;
			//繼續拜訪下一個子節點
			nextState = findNextSubNode();
			//如果該relocate有額外回合就會set pass_flag，且nextState不會是NULL，下一迴圈必處理到
			delete preState;
		}
		return bestv;
	}

	int maxValue() {
		if (terminal_test()) return terminal_utility();
		if (level >= deep) return utility();
		//node有successor才會進到以下
		//if (isTarget()) print();//t
		State* nextState = findNextSubNode();
		int bestv = INT32_MIN, nextv;
		while (nextState != NULL) {
			if (pass_flag) {
				pass_flag = 0;
				nextv = nextState->maxValue();
			}
			else nextv = nextState->minValue();
			//decisions[level] = -1;//t
			//if (isTarget()) value[actionQueue[actionQueue_id] - player * 7] = nextv;//t
			if (nextv > bestv) {
				bestv = nextv;
				if (pruning && bestv >= beta) {
					delete nextState;
					return bestv;
				}
				if (bestv > alpha) alpha = bestv;
			}
			State* preState = nextState;
			//繼續拜訪下一個子節點
			nextState = findNextSubNode();
			//如果該relocate有額外回合就會set pass_flag，且nextState不會是NULL，下一迴圈必處理到
			delete preState;
		}
		return bestv;
	}

public:

	int house[14];
	int player;

	//設初始狀態
	State(int house[14], int player, void (*scheme)(int[], State*) = NULL) {
		for (int i = 0; i < 14; i++)
			this->house[i] = house[i];
		this->player = player;
		level = 0;
		makeActionQueue = scheme;
		if (scheme != NULL) scheme(actionQueue, this);
		actionQueue_id = -1;
	}

private:
	//給pick產生下個state
	State(State* state, int pick) {
		for (int i = 0; i < 14; i++)
			this->house[i] = state->house[i];
		pass_flag = relocate(pick);
		this->player = (pass_flag == (pick >= 7));	//自動判斷現狀態為哪個玩家
		this->level = state->level + 1 + pass_flag;
		if (pruning) makeActionQueue(actionQueue, this);
		else for (int i = 0; i < 6; i++)
			actionQueue[i] = player * 7 + i;
		actionQueue_id = -1;
		this->alpha = state->alpha;
		this->beta = state->beta;
	}

public:

	void setPrioritySearchScheme(void (*scheme)(int[], State*)) {
		makeActionQueue = scheme;
	}

	//其實就是maxNode(root)，做maxValue()的工作，只不過root要求回傳的東西不一樣，故不能直接叫maxValue()，需另外定義
	//不能傳已是terminal的state
	int minimaxDecision(int deep) {
		decisionPlayer = player;
		this->deep = deep;
		pass_flag = 0;
		if (pruning) makeActionQueue(actionQueue, this);
		else for (int i = 0; i < 6; i++)
			actionQueue[i] = player * 7 + i;

		actionQueue_id = -1;

		alpha = INT32_MIN;
		beta = INT32_MAX;
		//以此state做為root搜尋第一層子節點
		//if (isTarget()) print();//t
		State* nextState = findNextSubNode();
		//最佳走法和最佳值
		int action = -1, nextv;
		while (nextState != NULL) {
			//拜訪到的子節點
			if (pass_flag) {
				pass_flag = 0;
				nextv = nextState->maxValue();
			}
			else nextv = nextState->minValue();
			//decisions[level] = -1;//t
			//if (isTarget()) value[actionQueue[actionQueue_id] - player * 7] = nextv;//t
			//紀錄最佳
			if (nextv > alpha) {
				alpha = nextv;
				//紀錄此子節點的from_action，即目前最佳走法
				action = actionQueue[actionQueue_id];
			}
			State* preState = nextState;
			//繼續拜訪下一個子節點
			nextState = findNextSubNode();
			//如果該relocate有額外回合就會set pass_flag，且nextState不會是NULL，下一迴圈必處理到
			delete preState;
		}
		//已找完所有第一層子節點
		return action;
	}

	void print() {
		cout << "[";
		for (int i = 0; i < 14; i++)
			cout << (i == 6 || i == 13 ? "{" : "") << house[i] << (i == 6 || i == 13 ? "} " : " ");
		cout << "\b]\n";
	}
};

int State::decisionPlayer = 0;
int State::deep = 0;
int State::pass_flag = 0;
void (*State::makeActionQueue)(int[], State*) = {};

void pure_order(int actionQueue[], State* const state) {
	for (int i = 0; i < 6; i++)
		actionQueue[i] = state->player * 7 + i;
}

void reverse_order(int actionQueue[], State* const state) {
	for (int i = 0; i < 6; i++)
		actionQueue[i] = state->player * 7 + 5 - i;
}

void extramoveFirst_sort(int actionQueue[], State* const state) {
	int priority[6];
	for (int i = 0; i < 6; i++) {
		int extra_move = 0;
		if ((i + state->house[state->player * 7 + i]) % 13 == 6) extra_move = 1;
		priority[i] = extra_move;
		actionQueue[i] = i;	//因stable sort，priority相等則以pure
	}
	for (int i = 1, j, tmp; i < 6; i++) {
		tmp = actionQueue[i];
		for (j = i - 1; j >= 0 && priority[tmp] > priority[actionQueue[j]]; j--)
			actionQueue[j + 1] = actionQueue[j];
		actionQueue[j + 1] = tmp;
	}
	if (state->player)
		for (int i = 0; i < 6; i++)
			actionQueue[i] += 7;
}

void emptycaptureFirst_sort(int actionQueue[], State* const state) {
	int priority[6];
	for (int i = 0; i < 6; i++) {
		int empty_capture = 0, seeds = state->house[state->player * 7 + i];
		int stop = (i + seeds) % 13, realstop = stop + state->player * 7;
		if (!state->house[realstop] && stop < 6 && (state->house[12 - realstop] || seeds >= 12 - stop - i))
			empty_capture = state->house[12 - realstop] + (seeds - 12 + stop + i + 1) / 13 + (seeds - (stop > i ? i - stop : stop - i + 13) + 1) / 13;
		priority[i] = empty_capture;
		actionQueue[i] = i;	//因stable sort，priority相等則以pure
	}
	for (int i = 1, j, tmp; i < 6; i++) {
		tmp = actionQueue[i];
		for (j = i - 1; j >= 0 && priority[tmp] > priority[actionQueue[j]]; j--)
			actionQueue[j + 1] = actionQueue[j];
		actionQueue[j + 1] = tmp;
	}
	if (state->player)
		for (int i = 0; i < 6; i++)
			actionQueue[i] += 7;
}

void storedFirst_sort(int actionQueue[], State* const state) {
	int priority[6];
	for (int i = 0; i < 6; i++) {
		int seeds = state->house[state->player * 7 + i];
		priority[i] = (seeds + i >= 6);
		actionQueue[i] = i;	//因stable sort，priority相等則以pure
	}
	for (int i = 1, j, tmp; i < 6; i++) {
		tmp = actionQueue[i];
		for (j = i - 1; j >= 0 && priority[tmp] > priority[actionQueue[j]]; j--)
			actionQueue[j + 1] = actionQueue[j];
		actionQueue[j + 1] = tmp;
	}
	if (state->player)
		for (int i = 0; i < 6; i++)
			actionQueue[i] += 7;
}

void comprehensive_sort(int actionQueue[], State* const state) {
	int priority[6];
	//計算priority，順便給初始actionQueue
	for (int i = 0; i < 6; i++) {	// i為自己的house id(不分玩家)
		int extra_move = 0, empty_capture = 0, seeds = state->house[state->player * 7 + i];
		int stop = (i + seeds) % 13, realstop = stop + state->player * 7;
		if (stop == 6) extra_move = 1;
		else if (!state->house[realstop] && stop < 6 && (state->house[12 - realstop] || seeds >= 12 - stop - i))
			empty_capture = state->house[12 - realstop] + (seeds - 12 + stop + i + 1) / 13 + (seeds - (stop > i ? i - stop : stop - i + 13) + 1) / 13;
		//絕對權重: extra_move > empty_capture > seeds = i
		priority[i] = extra_move * 10000 + empty_capture * 100 + (seeds + i >= 6);
		actionQueue[i] = 5 - i;
		//大部分比以下公式好些，但不全然
		//priority[i] = extra_move * 10000 + empty_capture * 100 + i;
		//actionQueue[i] = 5 - i;	//若priority不會相等，此行改為5-i便不會有差，只是稍微減少交換次數 加快排序速度而已
	}
	//對actionQueue插入排序
	for (int i = 1, j, tmp; i < 6; i++) {
		tmp = actionQueue[i];
		for (j = i - 1; j >= 0 && priority[tmp] > priority[actionQueue[j]]; j--)
			actionQueue[j + 1] = actionQueue[j];
		actionQueue[j + 1] = tmp;
	}
	//還原house id
	if (state->player)
		for (int i = 0; i < 6; i++)
			actionQueue[i] += 7;
}

int main() {
	int house[9][14] = {
		{ 0, 2, 3, 2, 1, 13, 17, 11, 0, 2, 9, 0, 0, 12 },
		{ 3, 0, 11, 0, 0, 1, 15, 1, 0, 10, 3, 0, 12, 16 },
		{ 7, 2, 15, 0, 1, 0, 5, 0, 5, 7, 6, 1, 14, 9 },
		{ 5, 0, 0, 16, 6, 3, 8, 0, 6, 13, 0, 0, 5, 10 },
		{ 11, 0, 2, 2, 2, 3, 9, 1, 0, 2, 12, 2, 12, 14 },
		{ 6, 6, 6, 6, 6, 6, 0, 0, 7, 7, 7, 7, 7, 1 },
		{ 7, 11, 4, 6, 10, 9, 4, 2, 5, 3, 4, 0, 1, 6 },
		{ 1, 11, 1, 10, 3, 2, 5, 2, 11, 11, 10, 1, 0, 4 },
		{ 0, 1, 0, 0, 2, 7, 25, 1, 1, 0, 0, 0, 2, 33 }
	};
	int player[9] = { 0, 1, 0, 1, 1, 1, 1, 0, 0 };
	void (*scheme[6])(int[], State*) = { pure_order, reverse_order, comprehensive_sort, extramoveFirst_sort,
										emptycaptureFirst_sort, storedFirst_sort };
	string testName[7] = { "Without Pruning", "Pure_order", "Reverse_order", "Comprehensive_sort",
							"ExtramoveFirst_sort", "EmptycaptureFirst_sort", "storedFirst_sort" };

	/* 單一測試
	State state(house[0], player[0], emptycaptureFirst_sort);
	visitNodes = 1;
	state.minimaxDecision(12);
	cout << visitNodes;
	return 0;*/

	cout << "\n" << setw(25) << "";
	for (int d = 8; d <= 32; d += 2)
		cout << right << setw(12) << "deep " + to_string(d);
	cout << "\n";
	for (int i = 0; i < 5; i++) {	//範例測資在 5 <= i <= 8
		cout << "\n";
		State state(house[i], player[i]);

		pruning = 0;
		cout << setw(25) << testName[0] + " |";
		for (int d = 8; d <= 14; d += 2) {	//without pruning 的測試深度
			visitNodes = 1;
			state.minimaxDecision(d);
			cout << right << setw(12) << visitNodes;
		}
		cout << "\n";

		pruning = 1;
		for (int s = 0; s < 3; s++) {	//策略有 0 <= s <= 5
			cout << setw(25) << testName[s + 1] + " |";
			state.setPrioritySearchScheme(scheme[s]);
			for (int d = 8; d <= (s == 2 ? 32 : (s == 1 ? 30 : 20)); d += 2) {	//各個策略的測試深度
				visitNodes = 1;
				state.minimaxDecision(d);
				cout << right << setw(12) << visitNodes;
			}
			cout << "\n";
		}
	}
}