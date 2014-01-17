import os
import sys


def activate():
    # Add the local src folder to the Python path
    sys.path.append(os.path.join(os.path.dirname(__file__), 'src'))
