# F Prime Project Governance

This governance model aims to create an open source community that encourages transparency, contributions, and collaboration, while maintaining our technical and quality standards. Our goal is to build a community comprised of members from private organizations, universities, government organizations, and international organizations that will collaborate towards the success of F Prime.

F Prime aims to create a product that accepts contributions from across the community and is overseen by a Change Control Board (CCB). The role of the CCB is to ensure that the development of the product is done in a way that meets the needs of the entire community by guiding its developments. The CCB is charged with the responsibility of determining where in the F Prime ecosystem work should be done, namely within the core F Prime products (fprime, fprime-tools, fprime-gds, fpp, and our tutorials), in a [fprime-community](https://github.com/fprime-community) add-on repository, or in project maintained code.

This structure is composed of various roles with various responsibilities. The F Prime Project will be hence referred to as "the Project".

> While F Prime intends to maintain high technical and quality standards, it is ultimately up to the user to ensure the software meet any specific guidelines or standards.

## Applicability

This Governance document applies to all F´ repositories hosted on NASA's GitHub Organization (fprime, fprime-tools, fprime-gds, and fpp). It also applies to  repositories in `fprime-community` directly managed by the Project (e.g. tutorial repositories). Other F´ add-on repositories are governed by their creators.

## Roles

The F Prime community breaks down into a set of roles that take actions and have certain privileges with respect to the development of F Prime.  These roles are:

| Role              | Available To        | Description                                                                                   |
|-------------------|---------------------|-----------------------------------------------------------------------------------------------|
| User              | Anyone              | Anyone downloading, deploying, or operating the software to meet a specific objective.        |
| Contributor       | Anyone              | Anyone providing input to the Project, including: code, issues, documentation, graphics, etc. |
| CCB Member        | Active, Sustained Contributor | Member of the change control board (CCB) guiding F Prime development.               |
| Community Manager | Active CCB Members  | Overall manager(s) of the product charged with execution of CCB guidance.                     |


Each role has a set of privileges afforded to it. These privileges are: 

| Role              | Read/Clone | Propose Pull Request | Comment in Tickets / Discussions | Review | Binding Review | Merge | Project Decisions |
|-------------------|------------|----------------------|----------------------------------|--------|----------------|-------|-------------------|
| User              | ✅          | ❌                    | ❌                                | ❌      | ❌              | ❌     | ❌                 |
| Contributor       | ✅          | ✅                    | ✅                                | ✅      | ❌              | ❌     | ❌                 |
| CCB Member        | ✅          | ✅                    | ✅                                | ✅      | ✅              | ❌     | ✅                 |
| Community Manager | ✅          | ✅                    | ✅                                | ✅      | ✅              | ✅     | ✅                  |

### User

Anyone who has downloaded, deployed, or used F Prime is a user. Users are free to read/clone the software. If a user comments or posts to the community the user becomes a Contributor. Users have complete authority over their projects.

### Contributor

Contributors include anyone that provides input to the Project. This includes code, issues, documentation, graphics, designs, or anything else that tangibly improves the Project. We encourage you to start contributing right away by joining our [Discussions](https://github.com/nasa/fprime/discussions) or submitting an [Issue](https://github.com/nasa/fprime/issues).

Contributors must follow the [code of conduct](https://github.com/nasa/fprime/blob/devel/CODE_OF_CONDUCT.md) and the [contributing guide](https://github.com/nasa/fprime/blob/devel/CONTRIBUTING.md).

### CCB Member

CCB members are individuals working as part of the governance of the F Prime team. These members are charged, as a team, to make decisions to guide the development of F Prime. These decisions influence how an [idea becomes a contribution](#the-decision-making-process).  CCB members also have the responsibility of performing binding reviews on Pull Requests before they can be merged into the product.

CCB members do not operate in a vacuum. These members solicit feedback from the community through conversations in our Discussions and Issues. Specifically, CCB members will open [Calls for Comments](https://github.com/nasa/fprime/discussions/categories/call-for-comments) to solicit feedback on specific decisions they will make.

The CCB is also charged with selecting the contributors to join the CCB.  These members must be active sustained members of the F Prime community.

### Community Manager 

A community manager is charged with carrying out the decisions of the CCB. This often means communicating back decisions, merging pull requests, and guiding Calls for Comments. The CCB is charged with selecting community managers. Pull requests are merged by Community Managers after receiving a Binding Review from at least one CCB Member.

Community managers are also members of the CCB and are thus involved in decisions of the Project as described above.

## The Decision-Making Process

F Prime has to support a number of community members both inside and outside of NASA. This means the Project must maintain high technical and quality standards, cybersecurity standard, and must be developed ensuring that the needs of the community are met.  The Change Control Board (CCB) is charged with making these decisions.

First, a Contributor comes up with an idea for F Prime. Ideas must be submitted as [Issues](https://github.com/nasa/fprime/issues). These issues are automatically placed in the "CCB" state waiting CCB approval. Contributors should take caution when developing changes that have not been approved by the CCB as they may be reworked or rejected.

Next, the CCB answers the question "How does this idea fit within the F Prime ecosystem?". For simple ideas (e.g. a bug report) this is often a quick decision to place this work within the core F Prime products.  For larger, or more breaking changes this decision can have a number of different outcomes:

1. A Call for Comments
2. A recommendation for modifications
3. Acceptance into F Prime
4. Acceptance as a community add-on
5. Recommendation for development within a users' projects

Each of these outcomes is elaborated on below. Contributors can check the status of an item on the [CCB Resolution Board](https://github.com/orgs/nasa/projects/21/views/17).

#### Call for Comments

When the CCB is asked to decide on an idea for F Prime for which the CCB lacks context, the idea only partially spelled-out, or the idea may result in wide-reaching changes impacting the community, a call for comments will be placed on the F Prime Discussions form.  The CCB will gather input on this idea from the community before reevaluating the idea.

#### A Recommendation for Modifications

The CCB may respond with modifications to an idea to help it fit better within the scope of the F Prime core products. Contributors receiving these recommendations should take them to heart as it often means this idea is wanted within F Prime and the CCB is helping the idea meet the standards of the Project.

#### Acceptance into F Prime

The CCB has accepted that this contribution belongs in F Prime's core products and development may begin. At this point issues move out of the CCB Resolution Board, however; individual issue records carry their resolution in the issue project metadata.

#### Acceptance as a Community Add-On

When the CCB has determined that an idea has merit but the applicability of the idea is limited to a subset of the community the CCB will recommend this be contributed as a community add-on. These add-ons are available to the community, and may be used by any number of projects.  They are shipped independently of the core products.  Most Operating System and Platform adaptations become community add-ons.

Community add-ons are not (with a few exceptions) maintained by the F Prime project.  However, should a community add-on have proven merit and code quality it can be accepted into the fprime-community organization. Add-ons may have designated maintainers who act as reviewers and stewards for that add-on.

#### Recommendation for Development Within a Users' Projects

Some ideas fit better within the context of a Users' projects. These contributions would have limited impact across the community, make broad and breaking changes, or need looser standards than the F Prime Project allows. Such ideas are often routed back to the User for implementation within their own code base.

### Bypassing the Process

As F Prime is open source, there is nothing stopping a Contributor from attempting to bypass the process by submitting a Pull Request without CCB approval. The Community Managers of F Prime do their best to accept these contributions, obtain CCB approval, and continue with development. However, this effort is not guaranteed and these contributions may be closed without feedback.  It is always best to follow the process outlined above.

## Communication Channels

The F Prime Project uses GitHub as a communication medium. Issues and Discussions will be communicated to using the conversation features provided by those mediums. The "Reviews" feature is used to communicate feedback to a specific contribution. The CCB will comment on Issues, and on Pull Requests to communicate feedback. The Community Manager will use the "Reviews" feature to provide the required pre-merge reviews.

[Announcements](https://github.com/nasa/fprime/discussions/categories/announcements) are used when F Prime needs to communicate to the entire community.
Calls for Comments are used as outlined above to solicit feedback from the community.

To communicate issues of cybersecurity, follow the [Security Policy](https://github.com/nasa/fprime/security/policy)

## Acknowledgements

The form of this document was modeled after the [SLIM Project](https://nasa-ammos.github.io/slim/) governance document.


