class Publisher(object):
    def __init__(self, topic, topic_type):
        self.topic = topic
        self.type = topic_type

    def publish(self, msg):
        assert isinstance(msg, self.type)
        print(msg)
