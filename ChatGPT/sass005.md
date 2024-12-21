Do you think Boost:Spirit can do the job?
ChatGPT

Thought about using Boost.Spirit for SAS interpreter for a second
Boost.Spirit can certainly help in building a parser for a language like SAS, at least for the subset of SAS you decide to handle. Boost.Spirit is a powerful C++ library for writing parsers directly in C++ using template metaprogramming and operator overloading. It allows you to write EBNF-like grammar rules inline in C++ code, which can be more succinct and declarative than hand-rolled lexers and parsers.

However, there are some important considerations:

Grammar Complexity:
SAS is a complex language with a rich set of features and somewhat irregular constructs¡ªespecially when you consider the DATA step, PROCs, macro language, informats, formats, and so forth. Writing a grammar that can handle all of SAS¡¯s quirks might still be challenging. If you narrow your scope to a simplified subset of SAS, Boost.Spirit can be very useful.

Learning Curve and Complexity:
Boost.Spirit uses template metaprogramming and can produce very complex error messages at compile time if you get the grammar wrong. It has a steeper learning curve compared to traditional parser generators or libraries like ANTLR. If you¡¯re comfortable with advanced C++ techniques and template-heavy code, Spirit can be a good choice. Otherwise, this complexity might slow you down.

Lexer Integration:
Spirit can function as a combined lexer and parser, or it can parse directly from input strings if your grammar rules are carefully constructed. However, with a language as large as SAS, you may prefer a separate tokenization phase for clarity and maintainability. Spirit¡¯s lex component (Qi + Lex) can be used for tokenization, but that adds another layer of complexity.

Performance:
Spirit is known to generate parsers that can be quite efficient since it uses inline template expansions, effectively ¡°generating¡± custom parsing code. For large SAS codebases, performance could be relatively good, assuming you structure the grammar and actions well.

Maintenance and Extensibility:
If you plan to evolve your grammar over time (adding new SAS language features or subsets), you may find Spirit¡¯s inline grammar definitions either very convenient (easy to modify in place) or challenging (complex templates can become hard to read). On the plus side, having the grammar close to the code can help keep the parser and semantic actions together, aiding maintainability if you have a small team of developers comfortable with the approach.

Conclusion:
Yes, Boost.Spirit can do the job of parsing a subset of SAS that you define. It is capable, flexible, and can result in efficient code. But the complexity of the SAS language, combined with the complexity of Spirit¡¯s templated approach, may make for a challenging implementation. If you are experienced with C++ template metaprogramming and find Spirit¡¯s style appealing, it can be a powerful tool. Otherwise, you might consider using a more traditional parser generator (like ANTLR with a C++ target) or a simpler hand-written parser approach.