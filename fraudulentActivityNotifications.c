int activityNotifications(int expenditure_count, int* expenditure, int d) {
    int freq[201] = {0};
    int notifications = 0;
    
    for(int i=0; i < d;i++){
        freq[expenditure[i]]++;
    }
    for(int i=d;i< expenditure_count;i++){
        int median2 =0;
        int count=0;
        if(d%2==1){
            for(int j=0; j <= 200;j++){
                count+=freq[j];
                if(count >d/2){
                    median2 = 2 * j;
                    break;
                }
            }
        } else {
            int m1 = -1, m2 = -1;
            for(int j=0; j <= 200;j++){
                count+=freq[j];
                if(m1 == -1 && count >=d/2) m1= j;
                if(count >=d/2+1){
                    m2 =j;
                    break;
                }
            }
            median2 = m1+m2;
        }
        if(expenditure[i]>=median2) notifications++;
        
        freq[expenditure[i-d]]--;
        freq[expenditure[i]]++;
    }
    return notifications;
}