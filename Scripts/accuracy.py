from sklearn.metrics import accuracy_score, recall_score, f1_score

labels_file = open("result.data", "r")
predictions_file = open("predictions.txt", "r")

labels = []
predictions = []

for line in labels_file:
    labels.append(int(line))

for line in predictions_file:
    predictions.append(int(line))

print(labels)
print(predictions)

acc = accuracy_score(labels, predictions)
print("Accuracy: ", acc)

recall = recall_score(labels, predictions)
print("Recall: ", recall)

f1 = f1_score(labels, predictions)
print("F1 Score: ", f1)
