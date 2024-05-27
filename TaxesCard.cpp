#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <chrono>
#include <random>
#include <iostream


//�Լ���������
const int INPUT_SELF_SIZE = 2;

//����������
const int INPUT_BOARD_SIZE = 5;

// ������������ȡ�Ƶ���ֵ
inline int get_card_value(int card) {
	return card & 0x000f;
}

// ������������ȡ�ƵĻ�ɫ
inline int get_card_suit(int card) {
	return card & 0x0f00;
}

// ö�����ͱ�ʾ��ͬ������
enum cardrank {
	royal_flush = 1,   // �ʼ�ͬ��˳
	straight_flush,    // ͬ��˳
	four_of_a_kind,    // ��գ�������
	full_house,        // ��«
	flush,             // ͬ��
	straight,          // ˳��
	three_of_a_kind,   // ����
	two_pair,          // ����
	one_pair,          // ����
	high_card,         // ����
	card_error = 100    //��������
};

// ��ʼ�����ܵ�˳�Ӽ���
static const std::set<std::vector<int>> straight_sets = {
	{2, 3, 4, 5, 6}, {3, 4, 5, 6, 7}, {4, 5, 6, 7, 8},
	{5, 6, 7, 8, 9}, {6, 7, 8, 9, 10}, {7, 8, 9, 10, 11},
	{8, 9, 10, 11, 12}, {9, 10, 11, 12, 13}, {10, 11, 12, 13, 14},
	{2, 3, 4, 5, 14} // a, 2, 3, 4, 5
};


// ԭʼ���Ƽ���
static const std::vector<int>  original_cards = {
0x102,0x103,0x104,0x105,0x106,0x107,0x108,0x109,0x10a,0x10b,0x10c,0x10d,0x10e,  // �ֱ������2,3,4,5,6,7,8,9,10,J,Q,K,A
0x202,0x203,0x204,0x205,0x206,0x207,0x208,0x209,0x20a,0x20b,0x20c,0x20d,0x20e,  //�ֱ����÷��2,3,4,5,6,7,8,9,10,J,Q,K,A
0x302,0x303,0x304,0x305,0x306,0x307,0x308,0x309,0x30a,0x30b,0x30c,0x30d,0x30e,  // �ֱ�������2,3,4,5,6,7,8,9,10,J,Q,K,A
0x402,0x403,0x404,0x405,0x406,0x407,0x408,0x409,0x40a,0x40b,0x40c,0x40d,0x40e,  //�ֱ�������2,3,4,5,6,7,8,9,10,J,Q,K,A
};



// ��ȡ���Ϻ���
std::vector<int> sample_elements(const std::vector<int>& original_set, size_t num_samples) {
	if (num_samples > original_set.size()) {
		throw std::invalid_argument("Number of samples exceeds the size of the original set");
	}

	std::vector<int> shuffled_set = original_set;

	// �����������
	std::random_device rd;
	std::mt19937 gen(rd());

	// ϴ��ԭʼ����
	std::shuffle(shuffled_set.begin(), shuffled_set.end(), gen);

	// ��ȡǰ num_samples ��Ԫ��
	std::vector<int> sampled_elements(shuffled_set.begin(), shuffled_set.begin() + num_samples);

	return sampled_elements;
}


// ����Ƿ�Ϊ˳��
bool is_straight(const std::vector<int>& values) {
	std::vector<int> sorted_values = values;
	std::sort(sorted_values.begin(), sorted_values.end());
	return straight_sets.find(sorted_values) != straight_sets.end();
}

// ����Ƿ�Ϊͬ��
bool is_flush(const std::vector<int>& suits) {
	return std::all_of(suits.begin(), suits.end(), [&](int suit) { return suit == suits[0]; });
}

// ����Ƿ�Ϊ�ʼ�ͬ��˳
bool is_royal_flush(const std::vector<int>& suits, const std::vector<int>& values) {
	return is_flush(suits) && std::set<int>(values.begin(), values.end()) == std::set<int>{10, 11, 12, 13, 14};
}

// �������
cardrank check_card_rank(const std::vector<int>& suits, const std::vector<int>& values) {
	std::unordered_map<int, int> value_count;
	for (int value : values) {
		value_count[value]++;
	}

	if (is_royal_flush(suits, values)) {
		return royal_flush; // �ʼ�ͬ��˳
	}
	if (is_flush(suits) && is_straight(values)) {
		return straight_flush; // ͬ��˳
	}
	if (std::any_of(value_count.begin(), value_count.end(), [](const std::pair<int, int>& p) { return p.second == 4; })) {
		return four_of_a_kind; // ��գ�������
	}
	bool has_three = std::any_of(value_count.begin(), value_count.end(), [](const std::pair<int, int>& p) { return p.second == 3; });
	bool has_two = std::any_of(value_count.begin(), value_count.end(), [](const std::pair<int, int>& p) { return p.second == 2; });
	if (has_three && has_two) {
		return full_house; // ��«
	}
	if (is_flush(suits)) {
		return flush; // ͬ��
	}
	if (is_straight(values)) {
		return straight; // ˳��
	}
	if (has_three) {
		return three_of_a_kind; // ����
	}
	int pair_count = std::count_if(value_count.begin(), value_count.end(), [](const std::pair<int, int>& p) { return p.second == 2; });
	if (pair_count == 2) {
		return two_pair; // ����
	}
	if (pair_count == 1) {
		return one_pair; // һ��
	}

	return high_card; // ����
}

// �����������������п��ܵ�5�������
void generate_combinations(const std::vector<int>& cards, std::vector<std::vector<int>>& all_combinations) {
	std::vector<bool> select = { true, true, true, true, true, false, false };

	do {
		std::vector<int> combination;
		for (size_t i = 0; i < cards.size(); ++i) {
			if (select[i]) {
				combination.push_back(cards[i]);
			}
		}
		all_combinations.push_back(std::move(combination));
	} while (std::prev_permutation(select.begin(), select.end()));
}

//�������н��
void generate_all_combinations(const std::vector<int>& vec, int n, int k, std::vector<std::vector<int>>& all_combinations) {
	std::vector<bool> v(n);
	std::fill(v.begin(), v.begin() + k, true);

	do {
		std::vector<int> combination;
		for (int i = 0; i < n; ++i) {
			if (v[i]) {
				combination.push_back(vec[i]);
			}
		}
		all_combinations.push_back(std::move(combination));
	} while (std::prev_permutation(v.begin(), v.end()));
}


// ���� f�������������
cardrank f(const std::vector<int>& self, const std::vector<int>& board) {

	auto self_size = self.size();
	auto board_size = board.size();
	if (!(self_size == INPUT_SELF_SIZE && board_size == INPUT_BOARD_SIZE)) {
		return card_error;
	}

	std::vector<int> cards;
	auto cards_size = self.size() + board.size();
	cards.reserve(cards_size);

	cards.insert(cards.end(), self.begin(), self.end());
	cards.insert(cards.end(), board.begin(), board.end());

	std::vector<std::vector<int>> all_combinations;
	generate_all_combinations(cards, cards_size, 5, all_combinations);

	cardrank best_rank = high_card;
	for (const auto& combination : all_combinations) {
		std::vector<int> values;
		std::vector<int> suits;
		for (int card : combination) {
			values.push_back(get_card_value(card));
			suits.push_back(get_card_suit(card));
		}
		cardrank rank = check_card_rank(suits, values);
		if (rank < best_rank) {
			best_rank = rank;
		}
	}

	return best_rank;
}

int main() {
	// ��¼��ʼʱ��
	auto start = std::chrono::high_resolution_clock::now();

	for (auto i = 0; i < 100; ++i) {

		std::vector<int> samples = sample_elements(original_cards, INPUT_SELF_SIZE + INPUT_BOARD_SIZE);
		std::vector<int> self(samples.begin(), samples.begin() + INPUT_SELF_SIZE);
		std::vector<int> board(samples.begin() + INPUT_SELF_SIZE, samples.end());

		std::cout << "slef cards: " << std::endl;
		for (const auto& card : self) {
			std::cout << "0x" << std::hex << card << " ";
		}
		std::cout << std::endl;

		std::cout << "board cards: " << std::endl;
		for (const auto& card : board) {
			std::cout << "0x" << std::hex << card << " ";
		}
		std::cout << std::endl;

		cardrank result = f(self, board);
		std::cout << "best self card rank: " << result << std::endl;
		std::cout << std::endl;
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "time took: " << duration << " milliseconds" << std::endl;

	return 0;
}
