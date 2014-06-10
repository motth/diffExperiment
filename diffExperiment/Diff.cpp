/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "Diff.h"

#include <QDebug>

Diff::Diff(git_repository* repository)
{
	Diff::repository_ = repository;

	Diff::oldTree_ = nullptr;
	Diff::newTree_ = nullptr;

	Diff::astDiff_ = new ASTDiff();

	Diff::options_ = new git_diff_options();
	initializeOptions();
}

void Diff::diffIndexToWorkdir()
{
	//TODO
	// git_diff_index_to_workdir
}

void Diff::diffTreeToWorkdir(git_tree* oldTree)
{
	//TODO
	(void) oldTree;
	// git_diff_tree_to_workdir_with_index
}

void Diff::diffTreeToIndex(git_tree* oldTree)
{
	//TODO
	(void) oldTree;
	// git_diff_tree_to_index
}

void Diff::diffTreeToTree(git_tree* oldTree, git_tree* newTree)
{
	Diff::diffKind_ = TreeToTree;

	Diff::oldTree_ = oldTree;
	Diff::newTree_ = newTree;

	int error = git_diff_tree_to_tree(&(Diff::diff_), Diff::repository_, Diff::oldTree_, Diff::newTree_, Diff::options_);
	checkError(error);

	Diff::astDiff_->buildFromGitDiff(Diff::diff_);

	Diff::astDiff_->print();
}


// Private methods

void Diff::checkError(int error)
{
	if (error < 0)
	{
		const git_error* lastError = giterr_last();
		qDebug() << "Error " << error << "/" << lastError->klass << ": " << lastError->message;
		exit(error);
	}
}

void Diff::initializeOptions()
{
	/**< version for the struct */
	Diff::options_->version = GIT_DIFF_OPTIONS_VERSION;
	/**< defaults to GIT_DIFF_NORMAL */
	Diff::options_->flags = GIT_DIFF_NORMAL;

	/* options controlling which files are in the diff */
	/**< submodule ignore rule */
	Diff::options_->ignore_submodules = GIT_SUBMODULE_IGNORE_DEFAULT;
	/**< defaults to include all paths */
	Diff::options_->pathspec = {NULL,0};
	Diff::options_->notify_cb = NULL;
	Diff::options_->notify_payload = NULL;

	/* options controlling how to diff text is generated */
	/**< defaults to 3 */
	Diff::options_->context_lines = 0;
	/**< defaults to 0 */
	Diff::options_->interhunk_lines = 0;
	 /**< default 'core.abbrev' or 7 if unset */
	Diff::options_->oid_abbrev = 7;
	/**< defaults to 512MB */
	Diff::options_->max_size = 536870912;
	/**< defaults to "a" */
	//options->old_prefix = 'a';
	/**< defaults to "b" */
	//options->new_prefix = 'b';
}
