import requests
from bs4 import BeautifulSoup
import pandas
import random

url = "https://www.mimuw.edu.pl/pl/aktualnosci/"

response = requests.get(url)
if response.status_code != 200:
    exit()

soup = BeautifulSoup(response.text, "html.parser")
event_divs = soup.find_all("div", class_="masonry-item news-archive-item")
events = []

for event_div in event_divs:
    title = event_div.find("h2").text.strip() if event_div.find("h2") else "N/A"
    date = event_div.find("div", class_="news-archive-item-date").text.strip() if event_div.find("div", class_="news-archive-item-date") else "N/A"
    link_tag = event_div.find("a", href=True)
    link = f"https://www.mimuw.edu.pl{link_tag['href']}" if link_tag else "N/A"
    events.append({"Title": title, "URL": link, "Date": date})

df = pandas.DataFrame.from_dict(events)
df.to_csv("mimuw_events.csv", index=False, encoding="utf-8")

random_events = random.sample(events, min(5, len(events)))
for event in random_events:
    print(f"Title: {event['Title']}, URL: {event['URL']}, Date: {event['Date']}")