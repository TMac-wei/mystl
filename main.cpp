#include <iostream>
#include <vector>

using namespace std;

class Solution{
public:
    int trap(vector<int>& height){
        if (height.size() < 2) {
            return 0;
        }

        vector<int> maxleft(height.size(), 0);
        vector<int> maxright(height.size(), 0);

        /// 先统计左边大的
        maxleft[0] = height[0];
        for (int  i = 1; i < height.size(); ++i) {
            maxleft[i] = max(maxleft[i - 1], height[i]);
        }

        maxright[height.size() - 1] = height[height.size() - 1];
        for (int i = height.size() - 2; i >= 0; i--) {
            maxright[i] = max(maxright[i + 1], height[i]);
        }

        int sum = 0;
        for (int i = 0; i < height.size(); i++) {
            int count = min(maxleft[i], maxright[i]) - height[i];
            if (count > 0) {
                sum += count;
            }
        }
        return sum;
    }
};

#include <algorithm>

int main()
{
    vector<int> height = {0,1,0,2,1,0,1,3,2,1,2,1};
    Solution so;
    std::cout << so.trap(height) << std::endl;
}