#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "repeat.hpp"

using namespace std;

class TableStyle {
public:
    TableStyle(std::string border = "|", 
               std::string dash = "-",
               std::string header_separator = "-",
               std::string header_left = "+",
               std::string header_right = "+",
               std::string tl_corner = "+",
               std::string tr_corner = "+",
               std::string bl_corner = "+",
               std::string br_corner = "+",
               char padding = ' ')
        : border_(border),
          dash_(dash),
          header_separator_(header_separator),
          header_left_(header_left),
          header_right_(header_right),
          tl_corner_(tl_corner),
          tr_corner_(tr_corner),
          bl_corner_(bl_corner),
          br_corner_(br_corner),
          padding_(padding) {}

    std::string get_border() const { return border_; }
    void set_border(std::string border) { border_ = border; }
    
    std::string get_dash() const { return dash_; }
    void set_dash(std::string dash) { dash_ = dash; }

    std::string get_header_separator() const { return header_separator_; }
    void set_header_separator(std::string header_separator) { header_separator_ = header_separator; }
    
    std::string get_header_left() const { return header_left_; }
    void set_header_left(std::string header_left) { header_left_ = header_left; }

    std::string get_header_right() const { return header_right_; }
    void set_header_right(std::string header_right) { header_right_ = header_right; }

    std::string get_corner(int pos) const { 
        switch (pos) {
        case 1: return tl_corner_;
        case 2: return tr_corner_;
        case 3: return bl_corner_;
        case 4: return br_corner_;
        }
        return "";
    }
    void set_corner(int pos, std::string corner) { 
        switch (pos) {
        case 1: tl_corner_ = corner; break;
        case 2: tr_corner_ = corner; break;
        case 3: bl_corner_ = corner; break;
        case 4: br_corner_ = corner; break;
        }
    }
    
    char get_padding() const { return padding_; }
    void set_padding(char padding) { padding_ = padding; }
    
private:
    std::string border_;
    std::string dash_;
    std::string header_separator_;
    std::string header_left_;
    std::string header_right_;
    std::string tl_corner_;
    std::string tr_corner_;
    std::string bl_corner_;
    std::string br_corner_;
    char padding_;
};

class Table {
public:
    Table(vector<vector<string>> data, TableStyle style = TableStyle()) : data_(data), style_(style) {}
    
    void set_headers(const vector<string>& headers, bool center = false) {
        headers_ = headers;
        header_centered_ = center;
        data_.insert(data_.begin(), headers_);
    }
    
    void add_row(const vector<string>& row) {
        data_.push_back(row);
    }
    
    void print() {
        // Determine the maximum width of each column
        vector<int> column_widths(data_[0].size());
        for (const auto& row : data_) {
            for (int i = 0; i < row.size(); i++) {
                column_widths[i] = max(column_widths[i], static_cast<int>(row[i].length()));
            }
        }
        
        // Calculate the total width of the table
        int table_width = 1; // start with 1 to account for the left border
        for (int width : column_widths) {
            table_width += width + 3; // add 3 to account for the padding and right border
        }
        
        // Print the top border
        cout << style_.get_corner(1) << repeatStr(style_.get_dash(), table_width - 2) << style_.get_corner(2) << endl;
        
        // Print the header row
        for (int i = 0; i < column_widths.size(); i++) {
            if (header_centered_) {
                int padding = column_widths[i] - headers_[i].length();
                int left_padding = padding / 2;
                int right_padding = padding - left_padding;
                cout << style_.get_border() << ' ' << string(left_padding, style_.get_padding()) << headers_[i] << string(right_padding, style_.get_padding()) << " ";
            } else {
                cout << style_.get_border() << ' ' << setw(column_widths[i]) << left << headers_[i] << " ";
            }
        }
        cout << style_.get_border() << endl;
        
        // Print the divider between header and data rows
        cout << style_.get_header_left() << repeatStr(style_.get_header_separator(), table_width - 2) << style_.get_header_right() << endl;
        
        // Print the table data
        for (int i = 1; i < data_.size(); i++) {
            for (int j = 0; j < data_[i].size(); j++) {
                cout << style_.get_border() << ' ' << setw(column_widths[j]) << left << data_[i][j] << " ";
            }
            cout << style_.get_border() << endl;
        }
        
        // Print the bottom border
        cout << style_.get_corner(3) << repeatStr(style_.get_dash(), table_width - 2) << style_.get_corner(4) << endl;
    }
    
private:
    vector<vector<string>> data_;
    vector<string> headers_;
    TableStyle style_;
    bool header_centered_ = false;
};

int main() {
    // Create a table style with a thick border and centered headers
    TableStyle style("║", "═", "═", "╠", "╣", "╔", "╗", "╚", "╝", ' ');
    
    // Create a table with sample data and print it
    Table table({{"Smith", "Zackery", "15", "Maine"}, {"Cooper", "Alice", "75", "Detroit"}, {"Mustaine", "Dave", "61", "California"}}, style);
    table.set_headers({"LName", "FName", "Age", "City"}, true);
    table.print();
    
    return 0;
}
