import re

def process_boundary_file(file_path, type_mapping):
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Шаблон для поиска блоков boundary
    pattern = re.compile(
        r'(\w+)\s*\n\s*\{\s*\n\s*type\s+(\w+);\s*\n\s*(physicalType\s+\w+;\s*\n\s*)?nFaces\s+(\d+);\s*\n\s*startFace\s+(\d+);\s*\n\s*\}',
        re.MULTILINE
    )
    
    def replace_match(match):
        name = match.group(1)
        new_type = type_mapping.get(name, match.group(2))  # Берем новый тип из mapping или оставляем старый
        # Собираем новый блок без physicalType
        return f"{name}\n    {{\n        type            {new_type};\n        nFaces          {match.group(4)};\n        startFace       {match.group(5)};\n    }}"
    
    # Заменяем все вхождения
    new_content = pattern.sub(replace_match, content)
    
    with open(file_path, 'w') as f:  # Перезаписываем исходный файл
        f.write(new_content)

# Пример использования:
type_mapping = {
    'inlet': 'patch',  # Замените на нужный тип
    'outlet': 'patch',
    'walls': 'wall',
    'wedge': 'wedge',
    # defaultFaces не меняем, если не нужно
}

process_boundary_file('constant/polyMesh/boundary', type_mapping)  # Изменяет файл напрямую