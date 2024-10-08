class Query:
    def __init__(self, id: str, title: str, year: str, authors: str, points: str):
        self.id = id
        self.title = title
        self.year = year
        self.authors = authors
        self.points = points

    def generate_sql(self):
        return f"INSERT INTO my_file (id, title, year, authors, points) VALUES ('{self.id}', '{self.title}', '{self.year}', '{self.authors}', '{self.points}');"

with open('publikacje.txt', 'r', encoding='utf-8') as file:
    file.readline()

    for line in file:
        line = line.strip()

        id: str = ""
        title: str = ""
        year: str = ""
        authors: str = ""
        points: str = ""

        comma: int = 0
        quotes: bool = False

        for i in range(len(line)):
            if comma == 0:
                if line[i] == ',':
                    comma += 1
                else:
                    id += line[i]

            elif comma == 1:
                if line[i] == '"' and not quotes:
                    quotes = True
                elif line[i] == '"' and quotes:
                    quotes = False
                elif line[i] == ',' and not quotes:
                    comma += 1
                elif line[i] == "'":
                    title += line[i]
                    title += line[i]
                else:
                    title += line[i]

            elif comma == 2:
                if line[i] == ',':
                    comma += 1
                else:
                    year += line[i]

            elif comma == 3:
                if line[i] == ',':
                    comma += 1
                else:
                    authors += line[i]

            elif comma == 4:
                if line[i] == ',':
                    comma += 1
                else:
                    points += line[i]

        query = Query(id, title, year, authors, points)
        print(query.generate_sql())