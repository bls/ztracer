
import unittest
from util import split_stream

def is_one(x):
    return x == 1

class TestSplitStream(unittest.TestCase):
    def test_empty_list(self):
        result = list(split_stream(iter([]), is_one))
        self.assertEqual(result, [])

    def test_delim_only(self):
        t = iter([1])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[1]])

        t = iter([1, 1])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[1],[1]])

        t = iter([1, 1, 1])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[1], [1], [1]])

    def test_no_delim(self):
        t = iter([2])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[2]])

        t = iter([2, 2])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[2, 2]])

        t = iter([2, 2, 2])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[2, 2, 2]])

    def test_happy_path(self):
        t = iter([1, 2, 3])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[1, 2, 3]])

        t = iter([1, 2, 3, 1, 2, 3])
        result = list(split_stream(t, is_one))
        self.assertEqual(result, [[1, 2, 3], [1, 2, 3]])

if __name__ == '__main__':
    unittest.main()
