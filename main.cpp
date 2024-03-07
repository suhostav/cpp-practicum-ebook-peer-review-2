#include <algorithm>
#include <cassert>
#include <ctype.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;


class Domain {
public:

    Domain( const string& name){
        SetName(name);
    }

    const string& GetName() const {
        return name_;
    }

    bool operator==(const Domain& other) const {
        return name_ == other.name_;
    }

    bool operator<(const Domain& other) const {
        return name_ < other.name_;
    }

    // метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& other) const{
        size_t M = other.name_.size();
        size_t N = name_.size();
        //субдомен должен быть длиннее домена на точку и хотябы 1 символ
        if(N < (M + 2)){
            return false;
        }
        return std::equal(name_.begin(), name_.begin() + M, other.name_.begin()) && 
            name_[M] == '.' &&
            name_[M + 1] != '.';
    }
private:
    string name_;

    void SetName(const string& name){
        name_ = name;
        transform(name_.begin(), name_.end(), name_.begin(), [](char c) {return (char)tolower(c);});
        std::reverse(name_.begin(), name_.end());
    }
};

class DomainChecker {
public:
    template <typename Iter>
    DomainChecker(Iter begin, Iter end): forbidden_domains_(begin, end){
        sort(forbidden_domains_.begin(), forbidden_domains_.end());
        auto last = unique(forbidden_domains_.begin(), forbidden_domains_.end(),
            []( Domain& lhs,  Domain& rhs){ return rhs.IsSubdomain(lhs);});
        forbidden_domains_.erase(last, forbidden_domains_.end());
    }
    const vector<Domain>& Get() const {
        return forbidden_domains_;
    }

    // метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain){
        int l = 0;
        int r = (int)forbidden_domains_.size() -1;
        int mid = (l + r) / 2;
        while(l <= r){
            if((domain == forbidden_domains_[(size_t)mid]) || domain.IsSubdomain(forbidden_domains_[(size_t)mid])){
                return true;
            }
            if(domain < forbidden_domains_[(size_t)mid]){
                r = mid - 1;
            } else {
                l = mid + 1;
            }
            mid = (l + r) / 2;
        }
        return false;
    }

private:
    std::vector<Domain> forbidden_domains_;
};

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

template <typename Number>
std::vector<Domain> ReadDomains(istream& in, Number num){
    std::vector<Domain> domains;
    string line;
    for(Number i = 0; i < num; ++i){
        getline(in, line);
        domains.emplace_back(line);
    }

    return domains;
}

void DomainTest(){
    {
        Domain d{"aBc.dE"s};
        assert(d.GetName() == "ed.cba"s);
        assert(d == "ABC.DE"s);
        assert(d < "XYZ.de"s);
    }
    {
        Domain d{"aBc.dE"s};
        Domain sd{"qw123.xy.abc.de"};
        Domain sd2{".abc.de"s};
        Domain sd3{"..abc.de"s};
        assert(sd.IsSubdomain(d));
        assert(!d.IsSubdomain(sd));
        assert(!sd2.IsSubdomain(d));
        assert(!sd3.IsSubdomain(d));
    }
}

void DomainCheckerTest(){
    vector<string> names{"gdz.ru", "maps.me", "m.gdz.ru", "com", "abc.de"};
    DomainChecker checker{names.begin(), names.end()};
    auto domains = checker.Get();
    assert(domains.size() ==4);
    assert( domains[0] == "abc.de"s &&
            domains[1] == "maps.me"s &&
            domains[2] == "com"s &&
            domains[3] == "gdz.ru"s );
    assert(checker.IsForbidden("gdz.ru"s));
    assert(checker.IsForbidden("gdz.com"s));
    assert(checker.IsForbidden("m.maps.me"s));
    assert(checker.IsForbidden("alg.m.gdz.ru"s));
    assert(checker.IsForbidden("maps.com"s));
    assert(!checker.IsForbidden("maps.ru"s));
    assert(!checker.IsForbidden("gdz.ua"s));
}

void StartTests(){
    DomainTest();
    DomainCheckerTest();
}

int main() {
    StartTests();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}