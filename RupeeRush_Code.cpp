#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <fstream>   
using namespace std;

class Stock 
{
private:
    string stockname;
    double price;
public:
    Stock(string stockname,double price) : stockname(stockname), price(price) {}
    string getstockname() const {return stockname;}
    double getPrice() const {return price;}
    void setPrice(double newPrice) {price=newPrice;}
    void fluctuatePrice() 
    {
        int direction=rand()%10; 
        double changePercent;
        if(direction<7) changePercent=(rand()%101)/1000.0;
        else changePercent=-1*((rand()%51)/1000.0);
        price+=price*changePercent;
        if(price<1.0) price=1.0;
    }
    void display() const {cout<<left<<setw(12)<<stockname<<" - INR "<<fixed<<setprecision(2)<<price<<endl;}
};

class User 
{
private:
    string name;
    double balance;
    double initialBalance;
    unordered_map<string, int> portfolio;
public:
    User(string a, double b):name(a),balance(b),initialBalance(b) {}
    double getBalance() const {return balance;}
    void savePortfolio() const 
    {
        ofstream file(name+"_portfolio.txt");
        if(!file) {cout<<"Error saving portfolio.\n";return;}
        file<<balance<<endl;
        for(const auto& entry:portfolio) {file<<entry.first<<" "<<entry.second<<endl;}
        file.close();
        cout<<"Portfolio saved successfully!\n";
    }

    void loadPortfolio() 
    {
        ifstream file(name+"_portfolio.txt");
        if(!file) return;
        file>>balance;
        string stockname;
        int qty;
        while(file>>stockname>>qty) {portfolio[stockname]=qty;}
        file.close();
        cout<<" Portfolio loaded for user: " << name << endl;
    }

    void buyStock(const Stock& stock, int quantity) 
    {
        double totalCost=stock.getPrice()*quantity;
        if(balance>=totalCost) 
        {
            balance-=totalCost;
            portfolio[stock.getstockname()]+=quantity;
            cout<<"Bought "<<quantity<<" shares of "<<stock.getstockname()<<" for INR "<<fixed<<setprecision(2)<<totalCost<<endl;
        } 
        else cout<<"Insufficient balance to buy.\n";
    }

    void sellStock(const Stock& stock,int quantity) 
    {
        string stockname=stock.getstockname();
        if(portfolio.count(stockname)==0 || portfolio[stockname]<quantity) {cout<<"Not enough shares to sell.\n";return;}
        double totalSale=stock.getPrice()*quantity;
        balance+=totalSale;
        portfolio[stockname]-=quantity;
        if(portfolio[stockname]==0) portfolio.erase(stockname);
        cout<<"Sold "<<quantity<<" shares of "<<stockname<<" for INR "<<fixed<<setprecision(2)<<totalSale<<endl;
    }

    void displayBalance() const {cout<<" Current Balance: INR "<<fixed<<setprecision(2)<<balance<<endl;}

    double calculatePortfolioValue(const vector<Stock>& marketStocks) const 
    {
        double totalValue=0;
        for(const auto& p:portfolio) 
        {
            const string& stockname=p.first;
            int quantity=p.second;
            for (const auto& stock : marketStocks) if(stock.getstockname()==stockname) totalValue+=stock.getPrice()*quantity;
        }
        return totalValue;
    }

    void displayPortfolio(const vector<Stock>& marketStocks) const 
    {
        cout<<"\n"<<name<<"'s Portfolio:\n";
        if(portfolio.empty()) 
        {
            cout<<"  (No holdings)\n";
            cout<<"   Risk Level: No investments yet.\n";
            return;
        }
        for(const auto& entry : portfolio) cout<<"  "<<entry.first<<": "<<entry.second<<" shares\n";
        int stockCount=portfolio.size();
        cout<<"\n Portfolio Diversification: "<<stockCount<<" stock(s)\n";
        double portfolioValue=calculatePortfolioValue(marketStocks);
        double totalWorth=balance+portfolioValue;
        double maxInv=0;
        for (const auto& p:portfolio) 
        {
            const string& stockname=p.first;
            int quantity=p.second;
            for(const auto& stock : marketStocks) 
            {
                if(stock.getstockname()==stockname) 
                {
                    double currInv=stock.getPrice()*quantity;
                    maxInv=max(maxInv,currInv);
                }
            }
        }
        double Concetration=(maxInv/totalWorth)*100.0;
        cout<<"\n Capital Concentration: "<<fixed<<setprecision(2)<<Concetration<<"% in top stock";
        if(Concetration>70) cout<<"\n Risk Level: HIGH - Too much capital in one stock!\n";
        else if(Concetration>40) cout << "\n Risk Level: MODERATE - Partially diversified.\n";
        else cout<<"\n Risk Level: LOW - Well diversified portfolio.\n";
        double profitLoss=totalWorth-initialBalance;
        cout<<"\n Portfolio Value: INR "<<fixed<<setprecision(2)<<portfolioValue;
        cout<<"\n Total Worth (Balance + Stocks): INR "<<totalWorth;
        if(profitLoss>0) cout<<"\n Profit: +INR " << profitLoss<<endl;
        else if(profitLoss<0) cout<<"\n Loss: INR "<<profitLoss<<endl;
        else cout<<"\n  No Profit, No Loss.\n";
    }
};

class TradingSystem 
{
private:
    vector<Stock> marketStocks;
public:
    void addStock(const Stock& stock) {marketStocks.push_back(stock);}
    void displayMarket() 
    {
        cout<<"\n--- Indian Stock Market ---\n";
        for(size_t i=0;i<marketStocks.size();i++) 
        {
            marketStocks[i].fluctuatePrice();
            cout<<i+1<<". ";
            marketStocks[i].display();
        }
    }
    Stock* getStockByIndex(int index) 
    {
        if(index<1 || index>(int)marketStocks.size()) return nullptr;
        return &marketStocks[index-1];
    }
    const vector<Stock>& getAllStocks() const {return marketStocks;}
};
 
int main() 
{
    srand(static_cast<unsigned int>(time(0)));
    TradingSystem Market;
    Market.addStock(Stock("RELIANCE",2800.50));
    Market.addStock(Stock("TCS",3700.75));
    Market.addStock(Stock("INFY",1450.20));
    Market.addStock(Stock("HDFCBANK",1600.00));
    Market.addStock(Stock("ITC",440.85));
    Market.addStock(Stock("HINDUNILVR",2700.00));
    Market.addStock(Stock("SBIN",620.90));
    Market.addStock(Stock("WIPRO",410.00));
    string s;
    cout<<"Enter your Username: ";
    cin>>s;
    cout<<"Welcome to RupeeRush, "<<s<<"!\n";
    User user(s,100000.00);
    user.loadPortfolio();
    int t;
    do 
    {
        cout<<"\n===== RupeeRush Trading Menu =====\n";
        cout<<"1. View Market\n2. Buy Stock\n3. Sell Stock\n4. View Portfolio\n5. View Balance\n6. Save Portfolio\n0. Exit\nEnter your choice: ";
        cin>>t;
        switch(t) 
        {
            case 1:
            {
                Market.displayMarket();
                break;
            }
            case 2: 
            {
                Market.displayMarket();
                int index,quantity;
                cout<<"Enter stock number to buy: ";
                cin>>index;
                cout<<"Enter quantity: ";
                cin>>quantity;
                Stock* stock=Market.getStockByIndex(index);
                if(stock) user.buyStock(*stock,quantity);
                else cout<<"Invalid selection.\n";
                break;
            }
            case 3: 
            {
                Market.displayMarket();
                int index, quantity;
                cout<<"Enter stock number to sell: ";
                cin>>index;
                cout<<"Enter quantity: ";
                cin>>quantity;
                Stock* stock=Market.getStockByIndex(index);
                if(stock) user.sellStock(*stock,quantity);
                else cout<<"Invalid selection.\n";
                break;
            }
            case 4:
            {
                user.displayPortfolio(Market.getAllStocks());
                break;
            }
            case 5:
            {
                user.displayBalance();
                break;
            }
            case 6:
            {
                user.savePortfolio();
                break;
            }
            case 0:
            {
                user.savePortfolio();
                cout<<" Portfolio saved. Exiting system. Goodbye!\n";
                break;
            }
            default: cout<<"Invalid choice.\n";
        }
    }while(t!=0);
return  0;

}
