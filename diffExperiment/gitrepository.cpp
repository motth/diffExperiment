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

#include "gitrepository.h"

#include <QDebug>

#include <diff.h>

GitRepository::GitRepository(const char* path)
{
	// Bugfix: initialize git threads is usually done automatically
	// but there seem to be a bug related to libgit2 and Qtf
	git_threads_init();


	GitRepository::path_ = path;

	GitRepository::repository_ = nullptr;
}

GitRepository::~GitRepository()
{
	// Bugfix: shutdown git threads is usually done automatically
	// but there seem to be a bug related to libgit2 and Qt
	git_threads_shutdown();
}

void GitRepository::open()
{
	int error = git_repository_open(&(GitRepository::repository_), GitRepository::path_);
	GitRepository::checkError(error);
}

void GitRepository::close()
{
	git_repository_free(GitRepository::repository_);
}


void GitRepository::diff()
{
	// Get commits
	int error;
	git_object* obj;

	// Get initial branch
	obj = nullptr;
	error = git_revparse_single(&obj, GitRepository::repository_, "initial^{commit}");
	GitRepository::checkError(error);
	git_commit* initialCommit = (git_commit*) obj;

	// Get modified branch
	obj = nullptr;
	error = git_revparse_single(&obj, GitRepository::repository_, "modified^{commit}");
	GitRepository::checkError(error);
	git_commit* modifiedCommit = (git_commit*) obj;


	// Print which commits were loaded
	const git_oid* oid;
	char shortsha[10] = {0};
	// initial SHA
	oid = git_commit_id(initialCommit);
	git_oid_tostr(shortsha, 9, oid);
	qDebug() << "Branch 'initial' points to commit" << shortsha;
	// modified SHA
	oid = git_commit_id(modifiedCommit);
	git_oid_tostr(shortsha, 9, oid);
	qDebug() << "Branch 'modified points to commit" << shortsha;


	// get the tree from the commits
	git_tree* initialTree = nullptr;
	git_commit_tree(&initialTree,initialCommit);

	git_tree* modifiedTree = nullptr;
	git_commit_tree(&modifiedTree,modifiedCommit);


	// create a diff
	Diff* diff = new Diff(GitRepository::repository_);
	diff->diffTreeToTree(initialTree, modifiedTree);
}




// Private methods

void GitRepository::checkError(int error)
{
	if (error < 0)
	{
		const git_error* lastError = giterr_last();
		qDebug() << "Error " << error << "/" << lastError->klass << ": " << lastError->message;
		exit(error);
	}
}
