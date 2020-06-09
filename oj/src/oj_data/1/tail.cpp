// 处理数据结果
vector<int> func1(std::vector<int> arr,size_t k){
    if(arr.empty() || k <= 0 || k > arr.size()){
        return vector<int>();
    }
    sort(arr.begin(),arr.end());
    return vector<int>(arr.begin(),arr.begin() + k);
}

// 测试用例数据
vector<vector<int>> vArr ={{3,2,1},{},{1},{0,1,2,1},{0,0,1,2,4,2,2,3,1,4},{0,0,1,3,4,5,0,7,6,7}};
vector<int> vK = {2,3,1,2,4,8};

// 测试函数
bool TestFunc(){
    Solution s;
    for(size_t i=0;i < vArr.size();i++){
        // 获取测试数据
        vector<int> arr = vArr[i];
        int k = vK[i];

        vector<int> compare_result = s.getLeastNumbers(arr,k);
        vector<int> result = func1(arr,k);

        if(result != compare_result){
            cout << "该组数据未通过测试：";
            for(const auto &index:arr){
               cout << index << " "; 
            }
            cout << endl;
            return false;
        }
    }
    return true;

}
// 测试用例2
int main(){
    bool ret = TestFunc();
    if(ret){
        cout << "恭喜通过所有测试用例" << endl;
    }
    else{
        cout << "很遗憾！" << endl;
    }
    return 0;
}
