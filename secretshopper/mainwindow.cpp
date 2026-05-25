#include "mainwindow.h"
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <algorithm>
#include <QInputDialog>
#include <QDateTime> // Важно для генерации случайных чисел
#include <QTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_resultLayout(nullptr), m_cartTotal(0) {

    qDebug() << "1: start";
    setWindowTitle("🛒 Тайный Покупатель");
    setMinimumSize(1100, 750);

    qDebug() << "2: applyGlobalStyles";
    applyGlobalStyles();

    qDebug() << "3: loadStores";
    loadStores();

    qDebug() << "4: loadProductsByCategory";
    loadProductsByCategory();

    qDebug() << "5: setupUI";
    setupUI();

    qDebug() << "6: showWelcomePage";
    showWelcomePage();

    qDebug() << "7: done";
}

// ========== CartItemWidget ==========
CartItemWidget::CartItemWidget(const CartItem& item, QWidget* parent)
    : QWidget(parent), m_item(item) {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);

    m_nameLabel = new QLabel(item.name);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    layout->addWidget(m_nameLabel, 3);

    auto* qtyLayout = new QHBoxLayout();
    m_minusBtn = new QPushButton("-");
    m_minusBtn->setFixedSize(28, 28);
    m_plusBtn = new QPushButton("+");
    m_plusBtn->setFixedSize(28, 28);

    m_qtyLabel = new QLabel(QString::number(item.quantity));
    m_qtyLabel->setAlignment(Qt::AlignCenter);
    m_qtyLabel->setStyleSheet("font-size: 14px; min-width: 35px; background: #f8f9fa; border-radius: 4px;");

    qtyLayout->addWidget(m_minusBtn);
    qtyLayout->addWidget(m_qtyLabel);
    qtyLayout->addWidget(m_plusBtn);
    layout->addLayout(qtyLayout, 1);

    connect(m_plusBtn, &QPushButton::clicked, [this]() {
        m_item.quantity += 1;
        m_qtyLabel->setText(QString::number(m_item.quantity));
        emit quantityChanged(m_item.name, m_item.quantity);
    });
    connect(m_minusBtn, &QPushButton::clicked, [this]() {
        if (m_item.quantity > 1) {
            m_item.quantity -= 1;
            m_qtyLabel->setText(QString::number(m_item.quantity));
            emit quantityChanged(m_item.name, m_item.quantity);
        }
    });

    m_priceLabel = new QLabel(QString("~%1 ₽").arg(item.basePrice * item.quantity, 0, 'f', 0));
    m_priceLabel->setStyleSheet("font-weight: bold; color: #27ae60; font-size: 14px; min-width: 70px;");
    layout->addWidget(m_priceLabel, 1);

    m_deleteBtn = new QPushButton("🗑️");
    m_deleteBtn->setStyleSheet("QPushButton { background: #e74c3c; color: white; border-radius: 6px; padding: 6px; } QPushButton:hover { background: #c0392b; }");
    m_deleteBtn->setVisible(false);
    m_deleteBtn->setFixedSize(32, 32);
    connect(m_deleteBtn, &QPushButton::clicked, [this]() { emit removeRequested(m_item.name); });
    layout->addWidget(m_deleteBtn);
}

void CartItemWidget::enterEvent(QEvent* event) { QWidget::enterEvent(event); m_deleteBtn->setVisible(true); }
void CartItemWidget::leaveEvent(QEvent* event) { QWidget::leaveEvent(event); m_deleteBtn->setVisible(false); }

// ========== MainWindow ==========

MainWindow::~MainWindow() {}

void MainWindow::applyGlobalStyles() {
    qApp->setStyleSheet(R"(
        QScrollBar:vertical { border: none; background: #f0f2f5; width: 10px; border-radius: 5px; }
        QScrollBar::handle:vertical { background: #bdc3c7; min-height: 20px; border-radius: 5px; }
        QScrollBar::handle:vertical:hover { background: #95a5a6; }
        QListWidget { outline: none; }
        QListWidget::item { border-bottom: 1px solid #ecf0f1; }
        QListWidget::item:hover { background: #f8f9fa; }
    )");
}

void MainWindow::loadStores() {
    Store s;

    s.name = "🏪 Пятёрочка"; s.address = "ул. Ленина, 10"; s.distance = 150; s.color = QColor("#e74c3c");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 89.0}, {"🍞 Хлеб белый", 42.0}, {"🍗 Куриное филе", 245.0}, {"🧀 Сыр твёрдый", 390.0}, {"🥔 Картофель", 45.0}, {"🍅 Томаты", 125.0}, {"🧂 Соль", 28.0}, {"🥛 Кефир 2.5%", 72.0}, {"🍞 Хлеб чёрный", 48.0}};
    s.loyaltyDiscount = {{"🥛 Молоко пастеризованное 3.2%", 10}, {"🍞 Хлеб белый", 5}};
    m_stores.append(s);

    s.name = "🏪 Магнит"; s.address = "пр. Мира, 45"; s.distance = 280; s.color = QColor("#f39c12");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 82.0}, {"🍞 Хлеб белый", 39.0}, {"🍗 Куриное филе", 229.0}, {"🧀 Сыр твёрдый", 365.0}, {"🥔 Картофель", 41.0}, {"🍅 Томаты", 115.0}, {"🧂 Соль", 25.0}, {"🥛 Кефир 2.5%", 68.0}, {"🍞 Хлеб чёрный", 45.0}};
    s.loyaltyDiscount = {{"🧀 Сыр твёрдый", 12}};
    m_stores.append(s);

    s.name = "🏪 Перекрёсток"; s.address = "ул. Гагарина, 8"; s.distance = 420; s.color = QColor("#27ae60");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 95.0}, {"🍞 Хлеб белый", 48.0}, {"🍗 Куриное филе", 255.0}, {"🧀 Сыр твёрдый", 410.0}, {"🥔 Картофель", 49.0}, {"🍅 Томаты", 135.0}, {"🧂 Соль", 30.0}, {"🥛 Кефир 2.5%", 75.0}, {"🍞 Хлеб чёрный", 50.0}};
    s.loyaltyDiscount = {{"🍗 Куриное филе", 15}, {"🍅 Томаты", 10}};
    m_stores.append(s);

    s.name = "🏪 Лента"; s.address = "ш. Космонавтов, 100"; s.distance = 350; s.color = QColor("#3498db");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 84.0}, {"🍞 Хлеб белый", 40.0}, {"🍗 Куриное филе", 235.0}, {"🧀 Сыр твёрдый", 375.0}, {"🥔 Картофель", 43.0}, {"🍅 Томаты", 120.0}, {"🧂 Соль", 26.0}, {"🥛 Кефир 2.5%", 70.0}, {"🍞 Хлеб чёрный", 44.0}};
    s.loyaltyDiscount = {{"🥛 Молоко пастеризованное 3.2%", 8}};
    m_stores.append(s);

    s.name = "🏪 Ашан"; s.address = "ул. Победы, 22"; s.distance = 480; s.color = QColor("#0055a4");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 79.0}, {"🍞 Хлеб белый", 35.0}, {"🍗 Куриное филе", 215.0}, {"🧀 Сыр твёрдый", 350.0}, {"🥔 Картофель", 38.0}, {"🍅 Томаты", 105.0}, {"🧂 Соль", 22.0}, {"🥛 Кефир 2.5%", 65.0}, {"🍞 Хлеб чёрный", 38.0}};
    s.loyaltyDiscount = {{"🥛 Молоко пастеризованное 3.2%", 5}, {"🍞 Хлеб белый", 7}};
    m_stores.append(s);

    s.name = "🏪 Метро"; s.address = "пр. Строителей, 5"; s.distance = 320; s.color = QColor("#2c3e50");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 75.0}, {"🍞 Хлеб белый", 38.0}, {"🍗 Куриное филе", 210.0}, {"🧀 Сыр твёрдый", 340.0}, {"🥔 Картофель", 36.0}, {"🍅 Томаты", 110.0}, {"🧂 Соль", 24.0}, {"🥛 Кефир 2.5%", 62.0}, {"🍞 Хлеб чёрный", 40.0}};
    s.loyaltyDiscount = {{"🧀 Сыр твёрдый", 8}};
    m_stores.append(s);

    s.name = "🏪 Монетка"; s.address = "ул. Кирова, 33"; s.distance = 190; s.color = QColor("#e67e22");
    s.prices = {{"🥛 Молоко пастеризованное 3.2%", 86.0}, {"🍞 Хлеб белый", 41.0}, {"🍗 Куриное филе", 238.0}, {"🧀 Сыр твёрдый", 370.0}, {"🥔 Картофель", 44.0}, {"🍅 Томаты", 118.0}, {"🧂 Соль", 27.0}, {"🥛 Кефир 2.5%", 71.0}, {"🍞 Хлеб чёрный", 43.0}};
    s.loyaltyDiscount = {{"🥛 Молоко пастеризованное 3.2%", 6}};
    m_stores.append(s);
}

void MainWindow::loadProductsByCategory() {
    auto addProd = [&](const QString& name, const QString& cat, double price, const QString& unit) {
        Product p{name, cat, price, unit};
        m_allProducts[name] = p;
        m_productsByCategory[cat].append(p);
    };

    // Молочные продукты
    addProd("🥛 Молоко пастеризованное 3.2%", "🥛 Молочные продукты", 85.0, "л");
    addProd("🥛 Кефир 2.5%", "🥛 Молочные продукты", 72.0, "л");
    addProd("🥛 Кефир 3.2%", "🥛 Молочные продукты", 78.0, "л");
    addProd("🥛 Ряженка 4%", "🥛 Молочные продукты", 75.0, "л");
    addProd("🥛 Йогурт натуральный", "🥛 Молочные продукты", 45.0, "шт");
    addProd("🥛 Йогурт фруктовый", "🥛 Молочные продукты", 52.0, "шт");
    addProd("🧀 Творог 5%", "🥛 Молочные продукты", 115.0, "упак");
    addProd("🧀 Творог 9%", "🥛 Молочные продукты", 125.0, "упак");
    addProd("🧀 Сыр твёрдый", "🥛 Молочные продукты", 380.0, "кг");
    addProd("🧀 Сыр плавленый", "🥛 Молочные продукты", 95.0, "упак");
    addProd("🧈 Масло сливочное 82.5%", "🥛 Молочные продукты", 185.0, "упак");
    addProd("🥛 Сметана 15%", "🥛 Молочные продукты", 78.0, "упак");
    addProd("🥛 Сметана 20%", "🥛 Молочные продукты", 85.0, "упак");
    addProd("🥛 Сливки 10%", "🥛 Молочные продукты", 65.0, "упак");
    addProd("🥛 Сгущённое молоко", "🥛 Молочные продукты", 88.0, "банка");

    // Хлеб и выпечка
    addProd("🍞 Хлеб белый", "🍞 Хлеб и выпечка", 42.0, "шт");
    addProd("🍞 Хлеб чёрный", "🍞 Хлеб и выпечка", 48.0, "шт");
    addProd("🍞 Хлеб бородинский", "🍞 Хлеб и выпечка", 52.0, "шт");
    addProd("🥐 Батон", "🍞 Хлеб и выпечка", 35.0, "шт");
    addProd("🥐 Круассаны", "🍞 Хлеб и выпечка", 65.0, "упак");
    addProd("🥐 Булочки с маком", "🍞 Хлеб и выпечка", 38.0, "шт");
    addProd("🥐 Булочки с изюмом", "🍞 Хлеб и выпечка", 42.0, "шт");
    addProd("🥯 Сушки ванильные", "🍞 Хлеб и выпечка", 55.0, "упак");
    addProd("🍪 Печенье овсяное", "🍞 Хлеб и выпечка", 85.0, "упак");
    addProd("🍪 Печенье песочное", "🍞 Хлеб и выпечка", 92.0, "упак");
    addProd("🍞 Лаваш армянский", "🍞 Хлеб и выпечка", 45.0, "шт");
    addProd("🥖 Багет", "🍞 Хлеб и выпечка", 58.0, "шт");

    // Мясо
    addProd("🍗 Куриное филе", "🥩 Мясо", 235.0, "кг");
    addProd("🍗 Курица целая", "🥩 Мясо", 185.0, "кг");
    addProd("🍗 Куриные ножки", "🥩 Мясо", 195.0, "кг");
    addProd("🍗 Куриные крылья", "🥩 Мясо", 175.0, "кг");
    addProd("🥩 Говядина", "🥩 Мясо", 480.0, "кг");
    addProd("🥩 Свинина", "🥩 Мясо", 350.0, "кг");
    addProd("🌭 Колбаса докторская", "🥩 Мясо", 270.0, "упак");
    addProd("🌭 Колбаса молочная", "🥩 Мясо", 285.0, "упак");
    addProd("🌭 Сосиски", "🥩 Мясо", 210.0, "упак");
    addProd("🌭 Сардельки", "🥩 Мясо", 225.0, "упак");
    addProd("🥓 Бекон", "🥩 Мясо", 320.0, "упак");
    addProd("🥓 Ветчина", "🥩 Мясо", 295.0, "упак");

    // Овощи
    addProd("🥔 Картофель", "🥗 Овощи", 45.0, "кг");
    addProd("🥕 Морковь", "🥗 Овощи", 38.0, "кг");
    addProd("🧅 Лук репчатый", "🥗 Овощи", 32.0, "кг");
    addProd("🧄 Чеснок", "🥗 Овощи", 185.0, "кг");
    addProd("🍅 Томаты", "🥗 Овощи", 125.0, "кг");
    addProd("🥒 Огурцы", "🥗 Овощи", 95.0, "кг");
    addProd("🍎 Яблоки", "🥗 Овощи", 85.0, "кг");
    addProd("🍌 Бананы", "🥗 Овощи", 75.0, "кг");
    addProd("🍊 Апельсины", "🥗 Овощи", 95.0, "кг");
    addProd("🍋 Лимоны", "🥗 Овощи", 145.0, "кг");
    addProd("🥑 Авокадо", "🥗 Овощи", 150.0, "шт");
    addProd("🫑 Перец болгарский", "🥗 Овощи", 145.0, "кг");
    addProd("🥬 Капуста", "🥗 Овощи", 42.0, "кг");
    addProd("🥬 Салат", "🥗 Овощи", 85.0, "упак");
    addProd("🥒 Кабачки", "🥗 Овощи", 65.0, "кг");
    addProd("🍆 Баклажаны", "🥗 Овощи", 95.0, "кг");

    // Бакалея
    addProd("🍚 Рис", "🍝 Бакалея", 85.0, "упак");
    addProd("🌾 Гречка", "🍝 Бакалея", 75.0, "упак");
    addProd("🍝 Макароны", "🍝 Бакалея", 55.0, "упак");
    addProd("🍝 Спагетти", "🍝 Бакалея", 68.0, "упак");
    addProd("🌾 Овсянка", "🍝 Бакалея", 52.0, "упак");
    addProd("🧂 Соль", "🍝 Бакалея", 25.0, "упак");
    addProd("🍬 Сахар", "🍝 Бакалея", 60.0, "упак");
    addProd("☕ Кофе молотый", "🍝 Бакалея", 280.0, "упак");
    addProd("🍵 Чай чёрный", "🍝 Бакалея", 95.0, "упак");
    addProd("🍵 Чай зелёный", "🍝 Бакалея", 105.0, "упак");
    addProd("🍯 Мёд", "🍝 Бакалея", 350.0, "банка");
    addProd("🥜 Орехи", "🍝 Бакалея", 450.0, "упак");
    addProd("🍓 Варенье", "🍝 Бакалея", 125.0, "банка");
    addProd("🥫 Тушёнка", "🍝 Бакалея", 185.0, "банка");

    // Бытовая химия
    addProd("🧴 Стиральный порошок", "🧴 Бытовая химия", 270.0, "упак");
    addProd("🧴 Гель для стирки", "🧴 Бытовая химия", 295.0, "упак");
    addProd("🧽 Средство для посуды", "🧴 Бытовая химия", 85.0, "шт");
    addProd("🧼 Мыло жидкое", "🧴 Бытовая химия", 120.0, "шт");
    addProd("🧼 Мыло туалетное", "🧴 Бытовая химия", 45.0, "шт");
    addProd("🚽 Средство для унитаза", "🧴 Бытовая химия", 140.0, "шт");
    addProd("🧹 Шампунь", "🧴 Бытовая химия", 195.0, "шт");
    addProd("🧴 Гель для душа", "🧴 Бытовая химия", 165.0, "шт");
    addProd("🧻 Туалетная бумага", "🧴 Бытовая химия", 180.0, "упак");
    addProd("🧻 Бумажные полотенца", "🧴 Бытовая химия", 95.0, "упак");
    addProd("🗑️ Мусорные пакеты", "🧴 Бытовая химия", 65.0, "упак");
    addProd("🧹 Средство для окон", "🧴 Бытовая химия", 115.0, "шт");
}

void MainWindow::setupUI() {
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);
    setCentralWidget(central);  // сначала устанавливаем central

    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(createWelcomePage());
    m_stackedWidget->addWidget(createProductSelectionPage());
    m_stackedWidget->addWidget(createStoreSelectionPage());
    m_stackedWidget->addWidget(createRouteOptimizationPage());

    mainLayout->addWidget(m_stackedWidget);
    createTopBar();  // topBar создаём ПОСЛЕ stacked
}

void MainWindow::createTopBar() {
    auto* bar = new QWidget();
    bar->setFixedHeight(65);
    bar->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1e3c72, stop:1 #2a5298); border-bottom: 2px solid rgba(255,255,255,0.1);");
    auto* lay = new QHBoxLayout(bar);
    lay->setContentsMargins(20,0,20,0);

    auto* logo = new QLabel("🛍️ Тайный Покупатель");
    logo->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    lay->addWidget(logo);
    lay->addStretch();

    auto* cartFrame = new QFrame();
    cartFrame->setStyleSheet("background: rgba(255,255,255,0.15); border-radius: 8px;");
    auto* cartLay = new QHBoxLayout(cartFrame);
    cartLay->setContentsMargins(12,6,12,6);

    cartLay->addWidget(new QLabel("🛒"));
    m_cartCountLabel = new QLabel("0");
    m_cartCountLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    cartLay->addWidget(m_cartCountLabel);

    lay->addStretch();
    cartLay->addWidget(new QLabel("Итого:"));
    m_cartTotalLabel = new QLabel("0 ₽");
    m_cartTotalLabel->setStyleSheet("color: #2ecc71; font-size: 16px; font-weight: bold;");
    cartLay->addWidget(m_cartTotalLabel);

    lay->addWidget(cartFrame);
    auto* mainLay = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    if (mainLay) mainLay->insertWidget(0, bar);
}

QWidget* MainWindow::createWelcomePage() {
    auto* w = new QWidget();
    w->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);");
    auto* l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);
    l->setSpacing(30);

    auto* icon = new QLabel("🛒");
    icon->setStyleSheet("background: white; border-radius: 100px; font-size: 80px; padding: 40px;");
    icon->setFixedSize(200, 200);
    l->addWidget(icon, 0, Qt::AlignCenter);

    auto* title = new QLabel("Умный шоппинг с оптимизацией!");
    title->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    l->addWidget(title);

    auto* subtitle = new QLabel("📍 Найдём лучшие цены\n💰 Сэкономим с картами лояльности\n🗺️ Построим оптимальный маршрут");
    subtitle->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 20px;");
    subtitle->setAlignment(Qt::AlignCenter);
    l->addWidget(subtitle);

    auto* btn = new QPushButton("🚀 Начать покупки");
    btn->setFixedSize(280, 65);
    btn->setStyleSheet("QPushButton { background: white; color: #764ba2; border-radius: 35px; font-size: 20px; font-weight: bold; border: none; } QPushButton:hover { background: #f8f9fa; }");
    connect(btn, &QPushButton::clicked, this, &MainWindow::showProductSelectionPage);
    l->addWidget(btn, 0, Qt::AlignCenter);

    return w;
}

QWidget* MainWindow::createProductSelectionPage() {
    auto* w = new QWidget();
    w->setStyleSheet("background: #f5f7fa;");
    auto* l = new QHBoxLayout(w);
    l->setContentsMargins(20,20,20,20);

    auto* left = new QFrame();
    left->setStyleSheet("background: white; border-radius: 15px;");
    auto* leftLay = new QVBoxLayout(left);
    leftLay->addWidget(new QLabel("📦 Категории"));

    auto* scroll = new QScrollArea();
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidgetResizable(true);
    auto* catWid = new QWidget();
    auto* catLay = new QHBoxLayout(catWid);
    m_categoryButtons = new QButtonGroup(this);

    QStringList cats = m_productsByCategory.keys();
    for (const auto& cat : cats) {
        auto* btn = new QPushButton(cat);
        btn->setCheckable(true);
        btn->setMinimumHeight(70);
        btn->setMinimumWidth(140);
        btn->setStyleSheet("QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #667eea, stop:1 #764ba2); color: white; border-radius: 10px; padding: 10px; } QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f093fb, stop:1 #f5576c); border: 2px solid #f5576c; }");
        connect(btn, &QPushButton::clicked, [this, cat]() { onCategoryClicked(cat); });
        catLay->addWidget(btn);
        m_categoryButtons->addButton(btn);
    }
    scroll->setWidget(catWid);
    leftLay->addWidget(scroll);

    m_productsList = new QListWidget();
    m_productsList->setStyleSheet("QListWidget { border: 2px solid #e9ecef; border-radius: 10px; background: #f8f9fa; font-size: 15px; } QListWidget::item { padding: 18px; border-radius: 8px; margin: 4px; background: white; border: 2px solid transparent; min-height: 50px; } QListWidget::item:hover { border: 2px solid #667eea; background: #e7f3ff; }");
    connect(m_productsList, &QListWidget::itemClicked, this, &MainWindow::onProductClicked);
    leftLay->addWidget(m_productsList);
    l->addWidget(left, 2);

    auto* right = new QFrame();
    right->setStyleSheet("background: white; border-radius: 15px;");
    auto* rightLay = new QVBoxLayout(right);
    rightLay->addWidget(new QLabel("🛒 Корзина"));

    m_cartList = new QListWidget();
    m_cartList->setStyleSheet("QListWidget { border: 2px solid #e9ecef; border-radius: 8px; background: #fafbfc; }");
    rightLay->addWidget(m_cartList);

    auto* btnLay = new QHBoxLayout();
    auto* clr = new QPushButton("🗑️ Очистить");
    clr->setStyleSheet("background: #e74c3c; color: white; border-radius: 8px; padding: 12px;");
    connect(clr, &QPushButton::clicked, this, &MainWindow::clearCart);
    btnLay->addWidget(clr);

    auto* next = new QPushButton("Выбрать магазины ➡");
    next->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #11998e, stop:1 #38ef7d); color: white; border-radius: 10px; padding: 15px; font-weight: bold;");
    connect(next, &QPushButton::clicked, this, &MainWindow::checkCartBeforeProceed);
    btnLay->addWidget(next);
    rightLay->addLayout(btnLay);
    l->addWidget(right, 2);

    showCategoryProducts("🥛 Молочные продукты");
    return w;
}

QWidget* MainWindow::createStoreSelectionPage() {
    auto* w = new QWidget();
    w->setStyleSheet("background: #f5f7fa;");
    auto* l = new QVBoxLayout(w);
    l->setContentsMargins(40,40,40,40);

    auto* title = new QLabel("📍 Выберите магазины в радиусе 100-500 метров");
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50;");
    title->setAlignment(Qt::AlignCenter);
    l->addWidget(title);

    auto* infoLabel = new QLabel("💡 Введите ваш адрес для автоматического расчёта расстояний до магазинов");
    infoLabel->setStyleSheet("color: #7f8c8d; font-size: 14px;");
    infoLabel->setAlignment(Qt::AlignCenter);
    l->addWidget(infoLabel);

    auto* addrLay = new QHBoxLayout();
    m_addressInput = new QLineEdit();
    m_addressInput->setPlaceholderText("🏠 Введите адрес (например: ул. Ленина, 15)");
    m_addressInput->setStyleSheet("padding: 12px; border: 2px solid #ddd; border-radius: 8px; font-size: 14px; background: white;");
    connect(m_addressInput, &QLineEdit::textChanged, this, &MainWindow::onAddressChanged);
    addrLay->addWidget(m_addressInput);

    auto* geoBtn = new QPushButton("📍 Геолокация");
    geoBtn->setStyleSheet("background: #3498db; color: white; border-radius: 8px; padding: 12px 20px; font-weight: bold;");
    connect(geoBtn, &QPushButton::clicked, this, &MainWindow::requestGeolocation);
    addrLay->addWidget(geoBtn);
    l->addLayout(addrLay);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    auto* cont = new QWidget();
    auto* grid = new QGridLayout(cont);
    m_storeCheckboxes.clear();

    int row = 0, col = 0;
    for (const auto& store : m_stores) {
        auto* chk = new QCheckBox(QString("%1\n📍 %2 (%3 м)").arg(store.name).arg(store.address).arg(store.distance));
        chk->setStyleSheet("font-size: 14px; padding: 15px; border: 2px solid #bdc3c7; border-radius: 10px; background: white;");
        chk->setChecked(false); // ИСПРАВЛЕНО: Галочки убраны по умолчанию
        m_storeCheckboxes[store.name] = chk;
        grid->addWidget(chk, row, col);
        if (++col > 2) { col = 0; ++row; }
    }
    scroll->setWidget(cont);
    l->addWidget(scroll);

    auto* btnLay = new QHBoxLayout();
    auto* back = new QPushButton("⬅ Назад к товарам");
    back->setStyleSheet("background: #95a5a6; color: white; border-radius: 8px; padding: 12px 25px; font-weight: bold;");
    connect(back, &QPushButton::clicked, this, &MainWindow::showProductSelectionPage);
    btnLay->addWidget(back);

    auto* calc = new QPushButton("🚀 Построить оптимальный маршрут");
    calc->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #667eea, stop:1 #764ba2); color: white; border-radius: 8px; padding: 15px 40px; font-size: 16px; font-weight: bold;");
    connect(calc, &QPushButton::clicked, this, &MainWindow::showRouteOptimizationPage);
    btnLay->addWidget(calc);
    l->addLayout(btnLay);
    return w;
}

QWidget* MainWindow::createRouteOptimizationPage() {
    auto* w = new QWidget();
    w->setStyleSheet("background: #f5f7fa;");
    auto* l = new QVBoxLayout(w);
    l->setContentsMargins(40,40,40,40);

    auto* title = new QLabel("✅ Оптимальный маршрут построен!");
    title->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #11998e, stop:1 #38ef7d); color: white; padding: 25px; border-radius: 12px; font-size: 26px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    l->addWidget(title);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    auto* content = new QWidget();
    m_resultLayout = new QVBoxLayout(content);
    l->addWidget(scroll);
    scroll->setWidget(content);

    auto* btnLay = new QHBoxLayout();
    auto* restart = new QPushButton("🔄 Начать заново");
    restart->setStyleSheet("background: #3498db; color: white; border-radius: 10px; padding: 15px 30px; font-size: 16px; font-weight: bold;");
    connect(restart, &QPushButton::clicked, this, &MainWindow::restartApp);
    btnLay->addWidget(restart);
    l->addLayout(btnLay);
    return w;
}

void MainWindow::showCategoryProducts(const QString& category) {
    m_productsList->clear();
    if (m_productsByCategory.contains(category)) {
        for (const auto& p : m_productsByCategory[category]) {
            m_productsList->addItem(QString("%1 — %2 ₽/%3").arg(p.name).arg(p.basePrice).arg(p.unit));
        }
    }
}

void MainWindow::onCategoryClicked(const QString& category) { showCategoryProducts(category); }

void MainWindow::onProductClicked(QListWidgetItem* item) {
    QString name = item->text().split(" — ")[0];
    addToCart(name);

    item->setBackgroundColor(QColor("#d5f5e3"));
    QTimer::singleShot(400, this, [item]() { item->setBackground(Qt::white); });
}

void MainWindow::addToCart(const QString& productName) {
    if (!m_allProducts.contains(productName)) return;
    const auto& p = m_allProducts[productName];

    bool exists = false;
    for (auto& ci : m_cart) {
        if (ci.name == productName) { ci.quantity += 1; exists = true; break; }
    }
    if (!exists) {
        m_cart.append({productName, 1, p.unit, p.basePrice});
    }
    refreshCartList();
    updateCartSummary();
}

void MainWindow::refreshCartList() {
    m_cartList->clear();
    for (const auto& ci : m_cart) {
        auto* wid = new CartItemWidget(ci);
        auto* lit = new QListWidgetItem();
        lit->setSizeHint(wid->sizeHint());
        connect(wid, &CartItemWidget::removeRequested, this, &MainWindow::onCartItemRemove);
        connect(wid, &CartItemWidget::quantityChanged, this, &MainWindow::onCartItemQuantityChange);
        m_cartList->addItem(lit);
        m_cartList->setItemWidget(lit, wid);
    }
}

void MainWindow::onCartItemRemove(const QString& name) {
    for (int i = m_cart.size() - 1; i >= 0; --i) {
        if (m_cart[i].name == name) {
            m_cart.removeAt(i);
            break;
        }
    }
    refreshCartList();
    updateCartSummary();
}

void MainWindow::onCartItemQuantityChange(const QString& name, double qty) {
    for (auto& ci : m_cart) if (ci.name == name) { ci.quantity = qty; break; }
    updateCartSummary();
}

void MainWindow::updateCartSummary() {
    m_cartCountLabel->setText(QString("%1 поз.").arg(m_cart.size()));
    double total = 0;
    for (const auto& ci : m_cart) total += ci.basePrice * ci.quantity;
    m_cartTotalLabel->setText(QString("%1 ₽").arg(total, 0, 'f', 0));
    m_cartTotal = total;
}

void MainWindow::clearCart() {
    if (m_cart.isEmpty()) { showErrorMessageBox("Корзина пуста!"); return; }
    if (QMessageBox::question(this, "Очистка", "Очистить корзину?") == QMessageBox::Yes) {
        m_cart.clear();
        refreshCartList();
        updateCartSummary();
    }
}

void MainWindow::checkCartBeforeProceed() {
    if (m_cart.isEmpty()) { showErrorMessageBox("Добавьте товары в корзину!"); return; }
    showStoreSelectionPage();
}

void MainWindow::requestGeolocation() {
    auto reply = QMessageBox::question(this, "Геолокация",
        "📍 Разрешить доступ к геопозиции?\n\n"
        "Это поможет определить ближайшие магазины в радиусе 100-500 метров.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Геолокация", "✅ Геолокация активирована!\n\nМагазины отсортированы по удалённости от вашего местоположения.");
        updateStoreDistances();
    }
}

void MainWindow::onAddressChanged(const QString& address) {
    if(address.length() < 3) return;
    updateStoreDistances();
}

void MainWindow::updateStoreDistances() {
    qsrand(QDateTime::currentMSecsSinceEpoch());

    for(auto &store : m_stores) {
        store.distance = 100 + (qrand() % 401);
    }

    std::sort(m_stores.begin(), m_stores.end(), [](const Store& a, const Store& b) {
        return a.distance < b.distance;
    });

    for (auto it = m_storeCheckboxes.begin(); it != m_storeCheckboxes.end(); ++it) {
        for (const auto& store : m_stores) {
            if (store.name == it.key()) {
                it.value()->setText(QString("%1\n📍 %2 (%3 м)").arg(store.name).arg(store.address).arg(store.distance));
                break;
            }
        }
    }
}

void MainWindow::calculateOptimalRoute() {
    m_selectedStores.clear();
    for (auto it = m_storeCheckboxes.begin(); it != m_storeCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) m_selectedStores.append(it.key());
    }

    if (m_selectedStores.isEmpty()) {
        showErrorMessageBox("Выберите хотя бы один магазин!");
        return;
    }

    if (m_resultLayout) {
        QLayoutItem* child;
        while ((child = m_resultLayout->takeAt(0)) != nullptr) { delete child->widget(); delete child; }
    }

    struct StoreAssignment {
        QString storeName;
        double distance;
        QList<QPair<QString, double>> products; // product, finalPrice
        double totalCost = 0;
    };

    QMap<QString, StoreAssignment> assignments;
    double totalWithoutOptimization = 0;
    double totalWithOptimization = 0;
    double totalSavings = 0;

    for (const auto& ci : m_cart) {
        QString bestStore;
        double minFinal = 999999.0;
        double regularPrice = ci.basePrice;

        for (const auto& sName : m_selectedStores) {
            const Store* s = nullptr;
            for (const auto& st : m_stores) if (st.name == sName) { s = &st; break; }

            if (s && s->prices.contains(ci.name)) {
                double price = s->prices[ci.name];
                double disc = s->loyaltyDiscount.value(ci.name, 0.0);
                double final = price * (1.0 - disc/100.0);

                if (final < minFinal) {
                    minFinal = final;
                    bestStore = sName;
                }
            }
        }

        if (!bestStore.isEmpty()) {
            assignments[bestStore].products.append(qMakePair(ci.name, minFinal * ci.quantity));
            assignments[bestStore].totalCost += minFinal * ci.quantity;
            totalWithOptimization += minFinal * ci.quantity;
            totalSavings += (regularPrice - minFinal) * ci.quantity;
        }
        totalWithoutOptimization += regularPrice * ci.quantity;
    }

    for (auto& assignment : assignments) {
        for (const auto& store : m_stores) {
            if (store.name == assignment.storeName) {
                assignment.distance = store.distance;
                break;
            }
        }
    }

    QStringList sortedStores = assignments.keys();
    std::sort(sortedStores.begin(), sortedStores.end(), [&assignments](const QString& a, const QString& b) {
        return assignments[a].distance < assignments[b].distance;
    });

    auto* savingsFrame = new QFrame();
    savingsFrame->setStyleSheet("QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #11998e, stop:1 #38ef7d); border-radius: 12px; padding: 20px; margin-bottom: 20px; }");
    auto* savingsLay = new QVBoxLayout(savingsFrame);

    auto* savingsTitle = new QLabel("💰 Ваша экономия:");
    savingsTitle->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    savingsLay->addWidget(savingsTitle);

    auto* savingsValue = new QLabel(QString("%1 ₽").arg(totalSavings, 0, 'f', 2));
    savingsValue->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");
    savingsLay->addWidget(savingsValue);

    auto* savingsInfo = new QLabel(QString("Без оптимизации: %1 ₽\nС оптимизацией: %2 ₽")
                                   .arg(totalWithoutOptimization, 0, 'f', 2)
                                   .arg(totalWithOptimization, 0, 'f', 2));
    savingsInfo->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 14px;");
    savingsLay->addWidget(savingsInfo);

    if (m_resultLayout) m_resultLayout->addWidget(savingsFrame);

    int step = 1;
    for (const auto& sName : sortedStores) {
        const auto& assignment = assignments[sName];

        const Store* sPtr = nullptr;
        for (const auto& st : m_stores) if (st.name == sName) { sPtr = &st; break; }

        auto* frame = new QFrame();
        frame->setStyleSheet(QString("QFrame { background: white; border-left: 6px solid %1; border-radius: 12px; padding: 20px; margin-bottom: 15px; }")
                             .arg(sPtr ? sPtr->color.name() : "#95a5a6"));
        auto* lay = new QVBoxLayout(frame);

        auto* header = new QHBoxLayout();
        auto* stepNum = new QLabel(QString("<b style='font-size: 24px; color: %1;'>%2</b>")
                                   .arg(sPtr ? sPtr->color.name() : "#95a5a6").arg(step++));
        header->addWidget(stepNum);

        auto* title = new QLabel(QString("🏪 %1").arg(sName));
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
        header->addWidget(title);
        header->addStretch();

        auto* distLabel = new QLabel(QString("📍 %1 м от вас").arg(assignment.distance));
        distLabel->setStyleSheet("color: #7f8c8d; font-size: 14px;");
        header->addWidget(distLabel);

        lay->addLayout(header);

        auto* productsLabel = new QLabel("<b style='font-size: 16px; color: #34495e;'>Купить здесь:</b>");
        lay->addWidget(productsLabel);

        for (const auto& item : assignment.products) {
            double qty = 1;
            for (const auto& ci : m_cart) {
                if (ci.name == item.first) { qty = ci.quantity; break; }
            }

            auto* productItem = new QLabel(QString("• %1 (x%2) — <b style='color: #27ae60;'>%3 ₽</b>")
                                          .arg(item.first).arg(qty).arg(item.second, 0, 'f', 2));
            productItem->setStyleSheet("font-size: 14px; padding: 4px 0;");
            lay->addWidget(productItem);
        }

        auto* totalLabel = new QLabel(QString("💵 Итого: <b style='font-size: 16px; color: #e74c3c;'>%1 ₽</b>")
                                     .arg(assignment.totalCost, 0, 'f', 2));
        totalLabel->setStyleSheet("margin-top: 10px; padding-top: 10px; border-top: 1px solid #ecf0f1;");
        lay->addWidget(totalLabel);

        if (m_resultLayout) m_resultLayout->addWidget(frame);
    }

    auto* totalFrame = new QFrame();
    totalFrame->setStyleSheet("QFrame { background: #2c3e50; border-radius: 12px; padding: 25px; }");
    auto* tLay = new QVBoxLayout(totalFrame);

    auto* totalTitle = new QLabel("💳 ОБЩАЯ СТОИМОСТЬ ПОКУПОК:");
    totalTitle->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 16px;");
    tLay->addWidget(totalTitle);

    auto* totalValue = new QLabel(QString("%1 ₽").arg(totalWithOptimization, 0, 'f', 2));
    totalValue->setStyleSheet("color: #2ecc71; font-size: 36px; font-weight: bold;");
    tLay->addWidget(totalValue);

    if (totalSavings > 0) {
        auto* saveLabel = new QLabel(QString("🎉 Вы сэкономили %1 ₽ благодаря умному распределению и картам лояльности!")
                                    .arg(totalSavings, 0, 'f', 2));
        saveLabel->setStyleSheet("color: #f39c12; font-size: 16px; margin-top: 10px;");
        tLay->addWidget(saveLabel);
    }

    if (m_resultLayout) { m_resultLayout->addWidget(totalFrame); m_resultLayout->addStretch(); }
}

void MainWindow::showErrorMessageBox(const QString& msg) { QMessageBox::warning(this, "Внимание", msg); }
void MainWindow::showWelcomePage() { m_stackedWidget->setCurrentIndex(0); }
void MainWindow::showProductSelectionPage() { m_stackedWidget->setCurrentIndex(1); }
void MainWindow::showStoreSelectionPage() { m_stackedWidget->setCurrentIndex(2); }
void MainWindow::showRouteOptimizationPage() { calculateOptimalRoute(); m_stackedWidget->setCurrentIndex(3); }
void MainWindow::restartApp() {
    m_cart.clear();
    m_selectedStores.clear();
    m_cartTotal = 0;
    updateCartSummary();
    refreshCartList();
    m_addressInput->clear();
    showWelcomePage();
}
