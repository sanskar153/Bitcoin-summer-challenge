
#include<iostream>
using namespace std;
typedef long long ll;

string input = "mempool.csv";
set<string> included_trx_ids;
int totalBlockWeight = 4000000;


class TransactionInfo {
    public:
        string tx_id;
        int fee;
        int weight;
        vector<string> parents;
};

pair<string, TransactionInfo*> addTransactionInfo(vector<string>& trxInfo){

    auto trx = new TransactionInfo();
    trx->tx_id = trxInfo[0];
    trx->fee = stoi(trxInfo[1]);
    trx->weight = stoi(trxInfo[2]);
    vector<string> pid;
    for (int i = 3; i < trxInfo.size(); i++){
        pid.push_back(trxInfo[i]);
    }
    trx->parents = pid;
    return {trxInfo[0], trx};

}

// read the cvs file

void readCSV(string filename, unordered_map<string, TransactionInfo*>& trx){

    ifstream fin(input);
    vector<string> trxDetails;
    string temp,line,word;
    getline(fin,line);
    while(getline(fin,line)){
        trxDetails.clear();
        stringstream s(line);
        while(getline(s,word,',')){
            trxDetails.push_back(word);
        }
        pair<string,TransactionInfo*> p = addTransactionInfo(trxDetails);
        trx[p.first] = p.second;
    }
    fin.close();

    cout << "Total Transactions Read: " << trx.size() << endl;

}

void getOutput(vector<string>& output, string filename){
    ofstream myfile(filename);
    for(auto s:output){
        myfile << s << endl;
    }
    myfile.close();
}

bool isValidTransaction(TransactionInfo* trx,set<string>& included_trx_ids){

    for(auto parent : trx->parents){
        if(included_trx_ids.find(parent) == included_trx_ids.end())
            return false;
    }
    return true;
}

// Main Function
int main(){

    unordered_map<string, TransactionInfo *> trx;
    readCSV(input, trx);
    set<pair<float, TransactionInfo *>, greater<pair<float, TransactionInfo *>>> transactionSet;
    set<string> transaction_set_included;
    vector<string> output;
    for(auto i:trx){
        transactionSet.insert({(float)i.second->fee / (float)i.second->weight, i.second});
    }
    int currentBlockWeight = 0;
    int totalFee = 0;
    while(!transactionSet.empty() && (currentBlockWeight < totalBlockWeight)){
        bool found = false;
        for(auto itr = transactionSet.begin(); itr != transactionSet.end(); itr++){
            TransactionInfo* currentTransaction = (*itr).second;
            int currFee = currentTransaction->fee;
            int currWeight = currentTransaction->weight;
            if(isValidTransaction(currentTransaction, included_trx_ids) && currentBlockWeight + currWeight <= totalBlockWeight){
                currentBlockWeight += currWeight;
                transaction_set_included.insert(currentTransaction->tx_id);
                output.push_back(currentTransaction->tx_id);
                totalFee += currFee;
                transactionSet.erase(itr);
                found = true;
                break;
            }
        }
        if(!found){
            break;
        }
    }
    cout << "Number of transaction in final block: " << transaction_set_included.size() << endl;
    cout << "Total fee in current block: " << totalFee << endl;
    cout << "Total weight in current block: " << currentBlockWeight << endl;
    getOutput(output, "block.txt");

    return 0;
}
