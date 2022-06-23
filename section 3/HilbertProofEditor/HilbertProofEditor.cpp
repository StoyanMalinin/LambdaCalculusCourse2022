#include <exception>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <vector>

using namespace std;

const string BOTTOM = "!";

struct Term
{
    virtual Term* clone() const = 0;
    virtual void print(ostream& os) const = 0;
    void println(ostream& os) const
    {
        print(os);
        os << '\n';
    }

    virtual bool isInFV(const string& varName) const = 0;

    string toString() const
    {
        stringstream s;
        print(s);

        return s.str();
    }

    virtual bool canMP(Term *antecedent) const
    {
        return false;
    }
    virtual Term* getSequent() const
    {
        return nullptr;
    }

    virtual bool canGen(const vector<Term*> &assumptions) const
    {
        return false;
    }
    virtual Term* getUnderQuantifier() const
    {
        return nullptr;
    }
};

bool cmpStrict(Term *t1, Term *t2)
{
    if(t1==nullptr) return false;
    if(t2==nullptr) return false;

    return (t1->toString()==t2->toString());
}

enum class Operation
{
    IMPLICATION,
    OR,
    AND,
    FOR_ALL,
    EXISTS,
    INVALID,
};

struct BinaryOperation : public Term
{
    Operation type;
    Term *lhs, *rhs;

    BinaryOperation(Operation type, Term *lhs, Term *rhs)
                    : type(type), lhs(lhs->clone()), rhs(rhs->clone())
    {}
    BinaryOperation(const BinaryOperation& other)
    {
        type = other.type;
        lhs = other.lhs->clone();
        rhs = other.rhs->clone();
    }

    Term* clone() const override
    {
        return new BinaryOperation(*this);
    }

    bool isInFV(const string& varName) const
    {
        return lhs->isInFV(varName) | rhs->isInFV(varName);
    }

    void print(ostream& os) const override
    {
        os << "(";
        lhs->print(os);
        os << ")";

        if(type==Operation::AND) os << "/\\";
        else if(type==Operation::OR) os << "\\/";
        else if(type==Operation::IMPLICATION) os << "->";

        os << "(";
        rhs->print(os);
        os << ")";

    }

    bool canMP(Term *antecedent) const override
    {
        return (type==Operation::IMPLICATION && cmpStrict(lhs, antecedent)==true);
    }

    Term* getSequent() const override
    {
        if(type==Operation::IMPLICATION) return rhs;
        return nullptr;
    }
};

struct UnaryOperation : public Term
{
    Operation type;
    string variable;
    Term *t;

    UnaryOperation(Operation type, const string& variable, Term *t)
                   : type(type), t(t->clone()), variable(variable)
    {}

    UnaryOperation(const UnaryOperation& other)
    {
        type = other.type;
        t = other.t->clone();
        variable = other.variable;
    }

    Term* clone() const override
    {
        return new UnaryOperation(*this);
    }

    bool isInFV(const string& varName) const override
    {
        if(varName!=variable)
            return t->isInFV(varName);

        return false;
    }

    void print(ostream& os) const override
    {
        if(type==Operation::FOR_ALL) os << "\\a";
        else if(type==Operation::EXISTS) os << "\\e";
        os << "_" << variable;

        os << " (";
        t->print(os);
        os << ")";
    }

    bool canGen(const vector<Term*> &assumptions) const override
    {
        if(type!=Operation::FOR_ALL) return false;


        for(Term *t: assumptions)
            if(t->isInFV(variable)==true) return false;

        return true;
    }

    Term* getUnderQuantifier() const  override
    {
        return t;
    }
};

struct AtomicFormula : public Term
{
    string name;

    AtomicFormula(const string& name) : name(name)
    {}

    Term* clone() const override
    {
        return new AtomicFormula(*this);
    }

    bool isInFV(const string& varName) const override
    {
        return false;
    }

    void print(ostream& os) const override
    {
        os << name;
    }
};

struct Variable : public Term
{
    string name;

    Variable(const string& name) : name(name)
    {}

    Term* clone() const override
    {
        return new Variable(*this);
    }

    bool isInFV(const string& varName) const override
    {
        return (name==varName);
    }

    void print(ostream& os) const override
    {
        os << name;
    }
};

struct Token
{
    virtual bool isBracket() const { return false; }
    virtual bool isLeftBracket() const { return false; }
    virtual bool isRightBracket() const { return false; }
    virtual bool isVariable() const { return false; }
    virtual bool isAtomicFormula() const { return false; }
    virtual bool isOperation() const {return false;}
    virtual bool isBinaryOperation() const { return false; }
    virtual bool isUnaryOperation() const { return false; }
    virtual Operation getOperationType() const { assert(false); }
    virtual string getName() const { assert(false); }
};

struct BinaryOperationToken : public Token
{
    Operation type;

    BinaryOperationToken(Operation type) : type(type) {}

    bool isOperation() const override { return true; }
    bool isBinaryOperation() const override { return true; }
    Operation getOperationType() const override { return type; }
};

struct UnaryOperationToken : public Token
{
    Operation type;
    string variable;

    UnaryOperationToken(Operation type, const string& variable) : type(type), variable(variable) {}

    bool isOperation() const override { return true; }
    bool isUnaryOperation() const override { return true; }
    string getName() const override { return variable; }
    Operation getOperationType() const override { return type; }
};

struct VariableToken : public Token
{
    string name;

    VariableToken(const string& name) : name(name) {}

    bool isVariable() const override { return true; }
    string getName() const override { return name; }
};

struct AtomicFormulaToken : public Token
{
    string name;

    AtomicFormulaToken(const string& name) : name(name) {}

    bool isAtomicFormula() const override { return true; }
    string getName() const override { return name; }
};

struct LeftBracketToken : public Token
{
    bool isBracket() const override { return true; }
    bool isLeftBracket() const override { return true; }
};

struct RightBracketToken : public Token
{
    bool isBracket() const override { return true; }
    bool isRightBracket() const override { return true; }
};

string removeSpaces(string expression)
{
    string res = "";
    for(char c: expression)
    {
        if(isspace(c)==false)
            res += c;
    }

    return res;
}

bool isLetter(char c)
{
    if('A'<=c && c<='Z') return true;
    if('a'<=c && c<='z') return true;
    if(c=='!') return true;
    return false;
}

vector <Token*> tokenize(string expression)
{
    vector<Token*> res;

    int n = expression.size();
    for(int i = 0;i<n;)
    {
        if(isspace(expression[i])!=0)
        {
            i++;
        }
        else if(isLetter(expression[i])==true)
        {
            string name = "";
            while(i<n && isLetter(expression[i])==true)
            {
                name += expression[i];
                i++;
            }

            if('a'<=name[0] && name[0]<='z') res.push_back(new VariableToken(name));
            else if('A'<=name[0] && name[0]<='Z') res.push_back(new AtomicFormulaToken(name));
            else assert(false);
        }
        else if(expression[i]=='(')
        {
            res.push_back(new LeftBracketToken());
            i++;
        }
        else if(expression[i]==')')
        {
            res.push_back(new RightBracketToken());
            i++;
        }
        else
        {
            if(expression[i]=='/')
            {
                assert(i+1<n && expression[i+1]=='\\');
                res.push_back(new BinaryOperationToken(Operation::AND));

                i += 2;
            }
            else if(expression[i]=='-')
            {
                assert(i+1<n && expression[i+1]=='>');
                res.push_back(new BinaryOperationToken(Operation::IMPLICATION));

                i += 2;
            }
            else if(expression[i]=='\\')
            {
                assert(i+1<n);

                if(expression[i+1]=='/')
                {
                    res.push_back(new BinaryOperationToken(Operation::OR));
                    i += 2;
                }
                else if(expression[i+1]=='a' || expression[i+1]=='e')
                {
                    char symbol = expression[i+1];
                    i += 2;

                    assert(i<n);
                    while(i<n && isspace(expression[i])!=0) i++;

                    assert(i<n);
                    string varName = "";
                    while(i<n && isLetter(expression[i])==true) {varName += expression[i];i++;}

                    if(symbol=='a')
                        res.push_back(new UnaryOperationToken(Operation::FOR_ALL, varName));
                    else
                        res.push_back(new UnaryOperationToken(Operation::EXISTS, varName));
                }
                else
                {
                    assert(false);
                }
            }
        }
    }

    return res;
}

bool isLowerPriority(Operation o1, Operation o2)
{
    if(o1==Operation::INVALID) return false;
    Operation order[] = {Operation::IMPLICATION, Operation::AND, Operation::OR, Operation::EXISTS, Operation::FOR_ALL};

    for(int i = 0;i<5;i++)
    {
        if(order[i]==o1) return true;
        if(order[i]==o2) return false;
    }

    assert(false);
}

Term *parseRec(int l, int r, const vector<Token*>& tokens)
{
    if(l>r)
        throw logic_error("invalid term!");

    if(l==r)
    {
        if(tokens[l]->isVariable()==true) return new Variable(tokens[l]->getName());
        if(tokens[l]->isAtomicFormula()==true) return new AtomicFormula(tokens[l]->getName());

        throw std::logic_error("invalid term!");
    }

    int bracketBalance = 0;
    vector <int> opPositions;

    for(int i = l;i<=r;i++)
    {
        if(tokens[i]->isLeftBracket()==true) bracketBalance++;
        else if(tokens[i]->isRightBracket()==true) bracketBalance--;

        if(bracketBalance<0) throw logic_error("invalid term!");
        if(bracketBalance==0 && tokens[i]->isOperation()==true)
            opPositions.push_back(i);
    }
    if(bracketBalance!=0)
        throw logic_error("invalid term!");

    if(opPositions.empty()==true)
    {
        if(!(tokens[l]->isLeftBracket()==true && tokens[r]->isRightBracket()==true))
            throw logic_error("invalid term!");

        return parseRec(l+1, r-1, tokens);
    }

    int lowestPriorityPos = opPositions[0];
    for(int i: opPositions)
        if(isLowerPriority(tokens[i]->getOperationType(), tokens[lowestPriorityPos]->getOperationType())==true)
            lowestPriorityPos = i;

    Operation splitOp = tokens[lowestPriorityPos]->getOperationType();
    if(splitOp==Operation::OR || splitOp==Operation::AND)
    {
        for(int i = opPositions.size()-1;i>=0;i--)
        {
            if(tokens[opPositions[i]]->getOperationType()==splitOp)
            {
                return new BinaryOperation(splitOp, parseRec(l, opPositions[i]-1, tokens),
                                                                parseRec(opPositions[i]+1, r, tokens));
            }
        }
    }
    else if(splitOp==Operation::IMPLICATION)
    {

        for(int i = 0;i<opPositions.size();i++)
        {
            if(tokens[opPositions[i]]->getOperationType()==Operation::IMPLICATION)
            {
                return new BinaryOperation(splitOp, parseRec(l, opPositions[i]-1, tokens),
                                                    parseRec(opPositions[i]+1, r, tokens));
            }
        }
    }
    else
    {
        assert(opPositions[0]==l);
        return new UnaryOperation(tokens[opPositions[0]]->getOperationType(), tokens[opPositions[0]]->getName(),
                                  parseRec(l+1, r, tokens));
    }
}

Term* parse(string expression)
{
    vector<Token*> tokens = tokenize(expression);

    Term *res = parseRec(0, tokens.size()-1, tokens);

    for(Token *ptr: tokens)
        delete ptr;

    return res;
}

Term* makeFormula(const string& msg)
{
    cout << msg << '\n';

    vector <Term*> formulas;
    for(int num = 1;;num++)
    {
        cout << ">";

        string cmd;
        cin >> cmd;

        if(cmd=="freestyle")
        {
            string expression;
            while(cin.peek()=='\n') cin.ignore();
            getline(cin, expression);

            Term *t = parse(expression);
            formulas.push_back(t);
        }
        else if(cmd=="combine")
        {

        }
        else if(cmd=="done")
        {
            if(formulas.empty()==true)
                cout << "no formula was made" << '\n';
            else
            {
                for(int i = 0;i<formulas.size()-1;i++) delete formulas[i];
                return formulas.back();
            }
        }
        else
        {
            cout << "invalid command" << '\n';
        }
    }
}

void enterAssumptions(vector <Term*> &assumptions)
{
    int assumptionCnt;
    cout << "Enter the number of assumptions: ";cin >>  assumptionCnt;

    cout << "Enter your assumptions" << '\n';
    for(int i = 1;i<=assumptionCnt;i++)
        assumptions.push_back(makeFormula("Enter assumption number " + to_string(i)));
}

int main()
{
    vector <Term*> assumptions;
    enterAssumptions(assumptions);

    cout << "Your assumptions are:" << '\n';
    for(int i = 0;i<assumptions.size();i++)
    {
        cout << i+1 << ". ";
        assumptions[i]->println(cout);
    }

    cout << '\n';
    cout << '\n';
    cout << '\n';
    cout << "The proof begins, good luck!" << '\n';

    vector <Term*> proof;
    for(int iter = 1;;iter++)
    {
        string type;
        cout << "Enter rule type (As/Ax/MP/Gen/QED): " << '\n';
        cout << ">";cin >> type;

        if(type=="As")
        {
            cout << "Enter assumption number: ";
            int num;cin >> num;

            proof.push_back(assumptions[num-1]->clone());
        }
        else if(type=="MP")
        {
            int j, k;
            cout << "j=";cin >> j;
            cout << "k=";cin >> k;

            if(proof[k-1]->canMP(proof[j-1])==true)
            {
                proof.push_back(proof[k-1]->getSequent()->clone());
            }
            else
            {
                cout << "Invalid (MP)" << '\n';
                continue;
            }
        }
        else if(type=="Gen")
        {
            int j;
            cout << "j=";cin >> j;

            proof[j-1]->println(cout);
            if(proof[j-1]->canGen(assumptions)==true)
            {
                proof.push_back(proof[j-1]->getUnderQuantifier()->clone());
            }
            else
            {
                cout << "Cannot (Gen)" << '\n';
                continue;
            }
        }
        else if(type=="QED")
        {
            break;
        }
        else
        {
            cout << "invalid type!" << '\n';
            continue;
        }

        cout << iter << ". ";
        proof.back()->println(cout);
    }

    cout << "You managed to prove: ";
    if(proof.empty()==false) proof.back()->println(cout);
    else cout << "\n";
}
