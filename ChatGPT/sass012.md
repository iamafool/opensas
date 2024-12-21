I don't want a parser generator. I need a library like boost spirit. Try again
ChatGPT

Thought about Boost.Spirit vs PEGTL for 9 seconds
If you¡¯re looking for a library similar to Boost.Spirit¡ªi.e., one that allows you to define your parser entirely in C++ without a separate code-generation step¡ªthen two strong candidates to consider are PEGTL (the Parsing Expression Grammar Template Library) and Lexy. Both are header-only libraries that integrate directly into your C++ codebase, let you define grammars declaratively, and do not require external parser generation steps.

PEGTL (Tao.CPP PEGTL)
Key Points:

Parsing Expression Grammars (PEGs): PEGTL uses PEGs, which are unambiguous and inherently deterministic. This means simpler reasoning about your grammar and fewer conflicts.
Header-only: Just include the headers and write your grammar rules as C++ template types.
Modern C++: Designed for C++17 and beyond, with clean interfaces and no heavy dependencies.
Clear Grammar Structure: Grammars are built as a hierarchy of rules, making it straightforward to incrementally develop and maintain the parser.
Why It¡¯s a Good Fit:

If you found Boost.Spirit¡¯s syntax elegant but complicated, PEGTL might feel more straightforward. Instead of operator overloading, PEGTL uses template specializations and nested types that closely mirror PEG syntax, leading to grammars that are easier to understand at a glance.
PEGTL offers precise control over error handling and diagnostics. You can write small tests for each grammar rule and build up to handle SAS-like complexity gradually.
Link:
PEGTL GitHub

Lexy
Key Points:

Combinator-based Grammar Definition: Lexy uses a declarative style somewhat similar to Spirit¡¯s combinators, but with a cleaner, more modern interface.
Focus on Error Handling: Lexy places strong emphasis on producing good diagnostics and error messages, which can be invaluable when dealing with a complicated language like SAS.
Zero Overhead Abstractions: The library focuses on performance and clarity, trying to minimize template complexity and provide better compile times compared to some older libraries.
Why It¡¯s a Good Fit:

If you liked the idea of Spirit¡¯s EBNF-like DSL but found it too heavy or complex, Lexy¡¯s approach might feel more approachable. It uses modern C++ techniques to keep grammars readable and maintainable.
Lexy makes it straightforward to handle both lexing and parsing in one framework, giving you structured error messages out of the box.
Link:
Lexy GitHub

In Summary:

PEGTL and Lexy are both excellent choices if you want a modern C++ library in the same vein as Boost.Spirit, but without going through a separate parser generator step.
PEGTL gives you PEG-based parsing, a well-structured grammar approach, and deterministic parsing behavior.
Lexy provides a fresh take on combinator-based parsing, focusing on clarity, error handling, and performance.
For a large and complex language like SAS, PEGTL might be the more straightforward choice due to its deterministic nature (no grammar conflicts) and clean PEG semantics. However, if you prefer a combinator style and rich error diagnostics, Lexy is a strong alternative. Both are good libraries to explore as you build your SAS interpreter.