class Config():
    def __init__(self):
        self.testset = 'test.csv'
        self.queries = 100000
        self.epochs = 100
        self.batch = 1000
        self.hid = 128
        self.cuda = True