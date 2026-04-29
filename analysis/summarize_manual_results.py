import pandas as pd

df = pd.read_csv('analysis/manual_review.csv')

print("=== Findings by Prompt Type ===")
print(df.groupby('prompt_type')['finding_type'].count())

print("\n=== Findings by Model ===")
print(df.groupby('model')['finding_type'].count())

print("\n=== Findings by Severity ===")
print(df.groupby('severity')['finding_type'].count())

print("\n=== Findings by Task ===")
print(df.groupby('task')['finding_type'].count())