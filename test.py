import requests
import sys
x = requests.post(f"http://{sys.argv[1]}/submit_letter", data={"letter":sys.argv[2]})