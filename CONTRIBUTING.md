# Contributing to Confidential Computing Explained

----------------------------------------------------

🚀🎉 Hello there! Thanks for taking the time to contribute to the ***Confidential Computing Explained Series***. 🚀🎉

The following is a set of guidelines for contributing to [the confidential computing series](https://github.com/mithril-security/Confidential_Computing_Explained.git) project. These are mostly guidelines, not rules. Use your best judgment, and feel free to propose changes to this document in a pull request.

#### Table Of Contents

[Code of Conduct](#code-of-conduct)

[What should I know before I get started?](#what-should-i-know-before-i-get-started)
  * [I only have a question!](#i-only-have-a-question)
  * [Confidential Computing Series](#confidential-computing-series)

[How Can I Contribute?](#how-can-i-contribute)
  * [Reporting Bugs](#reporting-bugs)
  * [Suggesting Enhancements](#suggesting-enhancements)
  * [Pull Requests](#pull-requests)
  * [Setting Your Local Development Environment](#setting-your-local-development-environment)

[Style Guidelines](#style-guidelines)
  * [Git Commit Messages](#git-commit-messages)
  * [Linting and Formatting](#linting-and-formatting)
  * [Pre-commit hook](#pre-commit-hook)

[Additional Notes](#additional-notes)
  * [Issue and Pull Request Labels](#issue-and-pull-request-labels)

## Code of Conduct

This project and everyone participating in it is governed by the [Mithril Security Code Of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [contact@mithrilsecurity.io](mailto:contact@mithrilsecurity.io).

## What should I know before I get started?

### I have a question or I want to discuss a topic
If you have a question to ask or you want to open a discussion about confidential computing in depth, we have a dedicated [Discord Community](https://discord.gg/TxEHagpWd4) in which all these kind of exchanges are more than welcome!

You can also follow the guidelines below to open an issue directly on the project. 

### Confidential Computing Series

The **Confidential Computing Series** are a set of tutorials and technical documents that aims to explain every side of Confidential Computing from beginner to expert. 

The purpose is to share our knowlegde, but also discover new intricate subjects all around confidential computing.  


## How Can I Contribute?
This section presents the different options that you can follow in order to contribute to BlindAI project. You can either **Report Bugs**, **Suggest Enhancements** or **Open Pull Requests**.

### Reporting Bugs
This section helps you through reporting Bugs for one of the **CC Explained** code. Following the guidelines helps the maintainers to understand your report, reproduce the Bug and work on fixing at as soon as possible. 

> **Important!**
> Before reporting a bug, please take a look at the [existing issues](https://github.com/mithril-security/Confidential_Computing_Explained/issues). You may find that the bug has already been reported and that you don't need to create a new one.

#### How to report a bug? 
To report a Bug, you can either:
- Follow this [link](https://github.com/mithril-security/Confidential_Computing_Explained/issues/new?assignees=gemini-15&labels=bug&template=bug_report.md&title=%5BBUG%5D) and fill the report with the required information.
- In BlindAI github repository:
  * Go to `Issues` tab.
  * Click on `New Issue` button.
  * Choose the `Bug` option.
  * Fill the report with the required information.

#### How to submit a good Bug Report?
- Follow the Bug Report template as much as possible (You can add further details if needed).
- Use a clear and descriptive title.
- Describe the expected behavior, the one that's actually happening and how often does it reproduce.
- Describe the exact steps which reproduce the problem.
- Specify the versions of BlindAI Client and Server that produced the bug.
- Add any other relevant information about the context, your development environment (Operating system, Language version ...)
- Attach screenshots, code snippets and any helpful resources.  

### Suggesting topics or asking questions about CC concepts 
This section guides you through suggesting subject ideas around Confidential Computing.
It can also be questions about detailing some concepts on confidential computing that you find interesting. You can do so by opening a **GitHub Issue**. 

> **Important!**
> Before opening an issue, please take a look at the [existing issues](https://github.com/mithril-security/Confidential_Computing_Explained/issues). You may find that the same suggestion has already been done and that you don't need to create a new one.

#### How to suggest a topic or a subject idea? 
To suggest an enhancement or a subject idea for an article or a tutorial you can either:

- Follow this [link](https://github.com/mithril-security/Confidential_Computing_Explained/issues/new/choose), choose the most relevant option and fill the report with the required information


#### How to submit a good topic suggestion?
- Choose the most relevant issue type for your suggestion.
- Follow the provided template as much as possible.
- Use a clear and descriptive title.
- Add any other relevant information about the context, your development environment (Operating system, Language version ...)
- Attach screenshots, code snippets and any helpful resources.  

### Pull Requests
This section helps you through the process of opening a Pull Request and contributing with code to the project!

#### How to open a pull request? 
In order to open a pull request:
- Go to Confidential_Computing_Explained GitHub repository.
- Fork the project.
- Do you magic! and push your changes. 
- Open a Pull Request
- Fill the description with the required information.

#### How to submit a good pull request?
- Make sure your pull request solves an open issue or fixes a bug. If no related issue exists, please consider opening an issue first so that we can discuss your suggestions. 
- Follow the [style guidelines](#style-guidelines). 
- Make sure to use a clear and descriptive title.
- Follow the instructions in the pull request template.
- Provide as many relevant details as possible.
- Make sure to [link the related issues](https://docs.github.com/en/issues/tracking-your-work-with-issues/about-issues#efficient-communication) in the description.

While the prerequisites above must be satisfied prior to having your pull request reviewed, the reviewer(s) may ask you to complete additional work, tests, or other changes before your pull request can be ultimately accepted.


## Style Guidelines

### Git Commit Messages

* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line


### Pre-commit hook
To ensure the style guidelines are maintained in the codebase, **pre-commit hook** is used.
Once cloning the repository, make sure to follow the following steps:

#### Install pre-commit package manager

```bash
pip install pre-commit
```
#### Install the git hooks script

```bash
pre-commit install
```
## Additional Notes

### Issue and Pull Request Labels

This section lists the labels we use to help us track and manage issues and pull requests.

[GitHub search](https://help.github.com/articles/searching-issues/) makes it easy to use labels for finding groups of issues or pull requests you're interested in.

The labels are organised in 4 groups : `Info`, `Type`, `Status` and `priority`.

The labels are loosely grouped by their purpose, but it's not required that every issue has a label from every group or that an issue can't have more than one label from the same group.

#### Issue and Pull Request Labels

| Label name | Description |
| --- | --- |
| Info : Client 🐍 | The Issue/PR affects the client-side |
| Info : Server 🦀 | The Issue/PR affects the server-side |
| Info : Build 🏗️ | The Issue/PR is related to the build process  |
| Info : Good First Issue 🎓 | Good for beginners and new incomers |
| Info : Duplicate❕ | The Issue/PR is duplicate |
| Info : Invalid 😕 | The Issue/PR doesn’t seem to be relevant |
| Type : Bug 🐞 | The Issue/PR reports/fixes a bug |
| Type : Refactor 🏭 |  The Issue/PR only refactors the codebase, not additional features of bug fixes are provided |
| Type : Improvement 📈 | The Issue/PR suggests an improvement of an existing functionality |
| Type : New Feature ➕ | The Issue/PR proposes a new feature that wasn’t in the codebase.  |
| Type : Documentation 📝 | The Issue/PR concerns the documentation (README, docstrings, CHANGELOG ...) |
| Type : Testing 🧪 | The Issue/PR adds, improves or edits tests. |
| Status : Available 🤚 | The Issue hasn’t been assigned yet |
| Status : In progress 👨‍🔧 | The work on the Issue/PR is in progress |
| Status : blocked 🚫 | The work on the Issue/PR is blocked by other tasks that haven’t been finished yet |
| Status : Completed 💯 | The work on the Issue/PR is completed |
| Status : Review needed 🙋‍♂️ | A review is needed in order to complete the work / approve it. |
| Status : To merge ✅ | Approved PR and will be merged!  |
| Priority : High 🔴 | The Issue is urgent, must be fixed as soon as possible |
| Priority : Medium 🟠 | The Issue is of a medium priority |
| Priority : Low 🟢 | The Issue is of a low priority and can wait a bit |