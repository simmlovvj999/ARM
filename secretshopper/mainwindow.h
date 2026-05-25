#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QButtonGroup>
#include <QMessageBox>
#include <QMap>
#include <QColor>
#include <QMouseEvent>
#include <QEvent>
#include <QTimer>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>

struct Product {
    QString name;
    QString category;
    double basePrice;
    QString unit;
};

struct Store {
    QString name;
    QString address;
    double distance; // в метрах
    QColor color;
    QMap<QString, double> prices; // Название товара -> Цена
    QMap<QString, double> loyaltyDiscount; // Название товара -> Скидка в %
};

struct CartItem {
    QString name;
    double quantity;
    QString unit;
    double basePrice;
};

class CartItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit CartItemWidget(const CartItem& item, QWidget* parent = nullptr);
    double getQuantity() const { return m_item.quantity; }
    QString getProductName() const { return m_item.name; }

signals:
    void removeRequested(const QString& productName);
    void quantityChanged(const QString& productName, double newQty);

private:
    CartItem m_item;
    QLabel* m_nameLabel;
    QLabel* m_qtyLabel;
    QLabel* m_priceLabel;
    QPushButton* m_deleteBtn;
    QPushButton* m_plusBtn;
    QPushButton* m_minusBtn;

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showWelcomePage();
    void showProductSelectionPage();
    void showStoreSelectionPage();
    void showRouteOptimizationPage();
    void onProductClicked(QListWidgetItem* item);
    void onCartItemRemove(const QString& productName);
    void onCartItemQuantityChange(const QString& productName, double newQty);
    void restartApp();
    void clearCart();
    void onCategoryClicked(const QString& category);
    void checkCartBeforeProceed();
    void requestGeolocation();
    void onAddressChanged(const QString& address);
    void calculateOptimalRoute();

private:
    QWidget* createWelcomePage();
    QWidget* createProductSelectionPage();
    QWidget* createStoreSelectionPage();
    QWidget* createRouteOptimizationPage();

    void setupUI();
    void createTopBar();
    void applyGlobalStyles();
    void loadProductsByCategory();
    void loadStores();
    void showCategoryProducts(const QString& category);
    void showErrorMessageBox(const QString& message);
    void addToCart(const QString& productName);
    void refreshCartList();
    void updateCartSummary();
    void updateStoreDistances();

    QStackedWidget *m_stackedWidget;
    QLabel *m_cartCountLabel;
    QLabel *m_cartTotalLabel;
    QListWidget *m_productsList;
    QListWidget *m_cartList;
    QButtonGroup *m_categoryButtons;
    QLineEdit *m_addressInput;
    QVBoxLayout *m_resultLayout;

    QMap<QString, QList<Product>> m_productsByCategory;
    QMap<QString, Product> m_allProducts;
    QList<Store> m_stores;
    QList<CartItem> m_cart;
    QMap<QString, QCheckBox*> m_storeCheckboxes;
    QStringList m_selectedStores;
    double m_cartTotal;
};

#endif // MAINWINDOW_H
