#ifndef PRODUCTLISTWIDGET_H
#define PRODUCTLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>

class ProductListWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProductListWidget(QWidget* parent = nullptr);
    QStringList products() const;
    void setProducts(const QStringList& list);
    void setAvailableProducts(const QStringList& products);

signals:
    void changed();

private slots:
    void addItem();
    void removeSelected();
    void addPreset();
    void clearAll();
    void onTextChanged(const QString& text);
    void onSuggestionClicked(QListWidgetItem* item);
    void updateProductCount();

private:
    QListWidget* m_list;
    QLineEdit* m_input;
    QComboBox* m_presets;
    QListWidget* m_suggestions;
    QLabel* m_productCountLabel;
    QLabel* m_headerIcon;
    QStringList m_availableProducts;

    void setupUI();
    void updateSuggestions(const QString& text);
    void applyModernStyle();
    QString getGradientButtonStyle(const QString& primaryColor, const QString& secondaryColor);
};

#endif // PRODUCTLISTWIDGET_H
