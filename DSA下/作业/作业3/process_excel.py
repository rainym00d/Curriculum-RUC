import pandas as pd

data = pd.read_excel('平面上2048个点.xlsx', header=None)
data.drop_duplicates(inplace=True)

data.to_csv('去重后数据.csv', header=None, index=False)

