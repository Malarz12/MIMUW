import json


def conversion(path):
    with open(path, 'r', encoding='utf-8') as file:
        data = json.load(file)

    new_cells = map(
        lambda cell:
        ''.join(map(lambda line:
        '# ' + line + '\n', cell['source'])) + '\n' if cell['cell_type'] == 'markdown'
        else ''.join(map(lambda line: line + '\n', cell['source'])) + '\n',
        data['cells'])

    count = sum(1 for _ in filter(lambda cell: cell['cell_type'] == 'markdown'
        and len(cell['source']) > 0
        and '# Ćwiczenie' in cell['source'][0],
    data['cells']))
    print("Liczba ćwiczeń: " + str(count))

    path = path[:-4] + 'py'
    with open(path , 'w', encoding='utf-8') as file:
        print(*new_cells, sep = '', file=file)


conversion('package.json')