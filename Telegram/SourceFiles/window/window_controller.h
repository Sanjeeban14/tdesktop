/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "mainwindow.h"
#include "window/window_adaptive.h"
#include "ui/layers/layer_widget.h"

namespace Main {
class Account;
class Session;
} // namespace Main

namespace Ui {
class Show;
} // namespace Ui

namespace Ui::Toast {
struct Config;
} // namespace Ui::Toast

namespace Media::View {
struct OpenRequest;
} // namespace Media::View

namespace Media::Player {
class FloatDelegate;
} // namespace Media::Player

namespace Window {

class Controller final : public base::has_weak_ptr {
public:
	Controller();
	explicit Controller(not_null<Main::Account*> account);
	Controller(
		not_null<PeerData*> singlePeer,
		MsgId showAtMsgId);
	~Controller();

	Controller(const Controller &other) = delete;
	Controller &operator=(const Controller &other) = delete;

	void showAccount(not_null<Main::Account*> account);
	[[nodiscard]] PeerData *singlePeer() const;
	[[nodiscard]] bool isPrimary() const {
		return (singlePeer() == nullptr);
	}

	[[nodiscard]] not_null<::MainWindow*> widget() {
		return &_widget;
	}
	[[nodiscard]] Main::Account &account() const {
		Expects(_account != nullptr);

		return *_account;
	}
	[[nodiscard]] Main::Account *maybeAccount() const {
		return _account;
	}
	[[nodiscard]] Main::Session *maybeSession() const;
	[[nodiscard]] SessionController *sessionController() const {
		return _sessionController.get();
	}
	[[nodiscard]] auto sessionControllerValue() const
		-> rpl::producer<SessionController*>;
	[[nodiscard]] auto sessionControllerChanges() const
		-> rpl::producer<SessionController*>;
	[[nodiscard]] bool locked() const;

	[[nodiscard]] Adaptive &adaptive() const;

	void finishFirstShow();

	void setupPasscodeLock();
	void clearPasscodeLock();

	void showLogoutConfirmation();

	void showSettings();

	[[nodiscard]] int verticalShadowTop() const;

	template <typename BoxType>
	QPointer<BoxType> show(
			object_ptr<BoxType> content,
			Ui::LayerOptions options = Ui::LayerOption::KeepOther,
			anim::type animated = anim::type::normal) {
		const auto result = QPointer<BoxType>(content.data());
		showBox(std::move(content), options, animated);
		return result;
	}

	void showToast(Ui::Toast::Config &&config);
	void showToast(TextWithEntities &&text, crl::time duration = 0);
	void showToast(const QString &text, crl::time duration = 0);

	void showLayer(
		std::unique_ptr<Ui::LayerWidget> &&layer,
		Ui::LayerOptions options,
		anim::type animated = anim::type::normal);

	void showRightColumn(object_ptr<TWidget> widget);

	void hideLayer(anim::type animated = anim::type::normal);
	void hideSettingsAndLayer(anim::type animated = anim::type::normal);
	[[nodiscard]] bool isLayerShown() const;

	void activate();
	void reActivate();
	void updateIsActiveFocus();
	void updateIsActiveBlur();
	void updateIsActive();
	void minimize();
	void close();

	void preventOrInvoke(Fn<void()> &&callback);

	void invokeForSessionController(
		not_null<Main::Account*> account,
		PeerData *singlePeer,
		Fn<void(not_null<SessionController*>)> &&callback);

	void openInMediaView(Media::View::OpenRequest &&request);
	[[nodiscard]] auto openInMediaViewRequests() const
	-> rpl::producer<Media::View::OpenRequest>;

	[[nodiscard]] QPoint getPointForCallPanelCenter() const;

	using FloatDelegate = Media::Player::FloatDelegate;
	void setDefaultFloatPlayerDelegate(
		not_null<Media::Player::FloatDelegate*> delegate);
	void replaceFloatPlayerDelegate(
		not_null<Media::Player::FloatDelegate*> replacement);
	void restoreFloatPlayerDelegate(
		not_null<Media::Player::FloatDelegate*> replacement);
	[[nodiscard]] FloatDelegate *floatPlayerDelegate() const;
	[[nodiscard]] auto floatPlayerDelegateValue() const
		-> rpl::producer<FloatDelegate*>;

	[[nodiscard]] std::shared_ptr<Ui::Show> uiShow();

	[[nodiscard]] rpl::lifetime &lifetime();

private:
	struct CreateArgs {
		PeerData *singlePeer = nullptr;
	};
	explicit Controller(CreateArgs &&args);

	void setupIntro(QPixmap oldContentCache);
	void setupMain(MsgId singlePeerShowAtMsgId, QPixmap oldContentCache);

	void showAccount(
		not_null<Main::Account*> account,
		MsgId singlePeerShowAtMsgId);
	void setupSideBar();
	void sideBarChanged();

	void showBox(
		object_ptr<Ui::BoxContent> content,
		Ui::LayerOptions options,
		anim::type animated);
	void checkThemeEditor();
	void checkLockByTerms();
	void showTermsDecline();
	void showTermsDelete();

	PeerData *_singlePeer = nullptr;
	Main::Account *_account = nullptr;
	base::Timer _isActiveTimer;
	::MainWindow _widget;
	const std::unique_ptr<Adaptive> _adaptive;
	std::unique_ptr<SessionController> _sessionController;
	rpl::variable<SessionController*> _sessionControllerValue;
	QPointer<Ui::BoxContent> _termsBox;

	rpl::event_stream<Media::View::OpenRequest> _openInMediaViewRequests;

	FloatDelegate *_defaultFloatPlayerDelegate = nullptr;
	FloatDelegate *_replacementFloatPlayerDelegate = nullptr;
	rpl::variable<FloatDelegate*> _floatPlayerDelegate = nullptr;

	rpl::lifetime _accountLifetime;
	rpl::lifetime _lifetime;

};

} // namespace Window
