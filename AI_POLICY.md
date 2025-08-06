# F´ Generative AI Usage Guidelines

We're excited about the potential of generative AI to help make [F´](https://github.com/nasa/fprime) development more productive, enjoyable, and accessible! Whether you're using AI to write code, improve documentation, or learn about complex systems, we welcome the thoughtful use of these powerful tools in your F´ contributions.

This guide shares our community's approach to using generative AI effectively and responsibly. You'll find practical tips, best practices, and simple guidelines to help you get the most out of AI tools while maintaining the quality standards that make F´ great.

## Our Position on Generative AI

F´ embraces technological advancement and innovation. Generative AI tools can assist with:

- Code generation and refactoring
- Documentation creation and improvement  
- Test case development
- Debugging assistance
- Design pattern suggestions
- Learning and understanding our codebases

However, the use of generative AI must align with our commitment to high technical standards, quality, and the collaborative nature of open source development.

## Disclosure

To maintain transparency and enable effective code review, contributors **must disclose all generative AI usage**.   
This includes contributions in the forms of **Pull Requests**, **Issues** or **Discussions**. 

### Pull Request Submissions for Contributors

1. **Fill-In the "AI Used (y/n)" table entry** in the pull request template disclosing whether Gen AI was used in the pull request
2. **Provide details in the "AI Usage" section** describing how generative AI was utilized

### What to Disclose

Include information about:

- **Type of assistance**: Code generation, documentation, debugging, testing, refactoring, etc.
- **Scope of usage**: Which files, functions, or sections were AI-assisted
- **Tool(s) used**: Name of the AI system(s) employed (e.g., GitHub Copilot, ChatGPT, etc.)
- **Level of modification**: Whether AI-generated content was used as-is, modified, or used as inspiration


### What AI Cannot Replace

- **Domain expertise** in flight software and embedded systems
- **Understanding of F Prime architecture** and design patterns
- **Critical thinking** about system requirements and constraints
- **Human judgment** on safety-critical decisions
- **Community collaboration** and peer review processes

## Best Practices

### Providing Guidelines to AI Tools

When working with generative AI, provide clear rules and context to improve code quality and consistency. For Example:

- **Reference F´ Style Guidelines**: Include the [F´ Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines) in your prompts
- **Enforce coding standards**: Instruct AI to avoid "magic numbers" and use descriptive variable names or comments
- **Provide project context**: Share relevant F´ architectural patterns and component structures

### Quality and Responsibility

- **Review all AI-generated code** thoroughly before submission
- **Verify necessity and relevance** - Remove verbose or unnecessary AI-generated content
- **Be concise** - Edit AI output to be clear and to-the-point
- **Ensure compliance** with F Prime coding standards and style guidelines
- **Verify correctness** and test all AI-assisted implementations
- **Maintain authorship responsibility** - you are accountable for all submitted code regardless of its origin

### Security

- **Be cautious with external dependencies** suggested by AI tools
- **Validate security implications** of AI-generated code, especially for flight software

### Code Review Considerations

- **Provide context** to reviewers about AI usage to enable informed evaluation
- **Be prepared to explain** AI-generated logic and design decisions
- **Accept feedback gracefully** - AI-generated code is not exempt from revision requests
- **Document complex AI-assisted algorithms** clearly for future maintainers

## Getting Help

If you have questions about appropriate AI usage or need guidance on disclosure:

- Open a [Discussion](https://github.com/nasa/fprime/discussions) for community input
- Contact the Community Managers for specific guidance
